#include "MercenaryStateComponent.h"

#include "Engine/DataTable.h"
#include "Engine/EngineTypes.h"

#include "Actors/Characters/MercenaryCharacter/MercenaryCharacter.h"
#include "Actors/Controllers/PlayerController/RPGPlayerController.h"


#include "Single/GameInstance/FRGameInstance.h"
#include "Single/PlayerManager/PlayerManager.h"

#include "Widgets/ClosableWnd/DraggableWnd/MercenaryShopWnd/MercenaryShopWnd.h"
#include "Widgets/HpableCharacterWidget/PlayerCharacterWidget/PlayerCharacterWidget.h"
#include "Widgets/ClosableWnd/MercenaryHpWnd/MercenaryHpWnd.h"

#include "Structures/MercenaryBlueprint/MercenaryBlueprint.h"


UMercenaryStateComponent::UMercenaryStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MERCENARY_INFO(
		TEXT("DataTable'/Game/Resources/DataTables/DT_MercenaryInfo.DT_MercenaryInfo'"));
	if (DT_MERCENARY_INFO.Succeeded()) DT_MercenaryInfo = DT_MERCENARY_INFO.Object;
	else UE_LOG(LogTemp, Error, TEXT("UMercenaryStateComponent.cpp::%d::LINE:: DT_MERCENARY_INFO is not loaded!"), __LINE__);

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MERCENARY_BP(
		TEXT("DataTable'/Game/Resources/DataTables/DT_MercenaryBlueprint.DT_MercenaryBlueprint'"));
	if (DT_MERCENARY_BP.Succeeded()) DT_MercenaryBP = DT_MERCENARY_BP.Object;
	else UE_LOG(LogTemp, Error, TEXT("UMercenaryStateComponent.cpp::%d::LINE:: DT_MERCENARY_BP is not loaded!"), __LINE__);
}


void UMercenaryStateComponent::BeginPlay()
{
	Super::BeginPlay();

	GameInst = Cast<UFRGameInstance>(GetWorld()->GetGameInstance());

	InitializeScoutMercenarys();
}


void UMercenaryStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMercenaryStateComponent::InitializeScoutMercenarys()
{
	if (GameInst->GetNextLevelName().IsNone()) return;

	UPlayerManager* playerManager = GetManager(UPlayerManager);

	if (playerManager->GetMercenaryInfo().Num() == 0) return;

	if (GameInst->GetNextLevelName() == FName(TEXT("GameMap")))
	{
		for (int i = 0; i < playerManager->GetMercenaryInfo().Num(); ++i)
			ScoutMercenary(playerManager->GetMercenaryInfo()[i].MercenaryCode);
	}
}

void UMercenaryStateComponent::ScoutMercenary(FName mercenaryCode)
{
	FString contextString;
	FMercenaryInfo* mercenaryInfo = DT_MercenaryInfo->FindRow<FMercenaryInfo>(
		mercenaryCode, contextString);

	FMercenaryBlueprint* mercenaryblueprint = DT_MercenaryBP->FindRow<FMercenaryBlueprint>(
		mercenaryCode, contextString);

	ScoutMercenarys.Add(FMercenarySlotInfo(mercenaryCode));

	auto capsuleComponent = Cast<ACharacter>(GetOwner())->GetCapsuleComponent();

	AMercenaryCharacter* mercenaryCharacter =
		GetWorld()->SpawnActor<AMercenaryCharacter>(
			mercenaryblueprint->MercenaryBlueprint,
			GetOwner()->GetActorLocation() + (FVector::BackwardVector * (capsuleComponent->GetScaledCapsuleHalfHeight() * 2.0f)),
			FRotator::ZeroRotator);

	MercenaryActors.Add(mercenaryCharacter);

	ScoutMercenaryInfo.Add(*mercenaryCharacter->GetMercenaryInfo());

	GetManager(UPlayerManager)->GetPlayerController()->GetPlayerCharacterWidgetInstance()->
		GetMercenaryHpWnd()->AddMercenaryHpList(mercenaryCharacter->GetMercenaryInfo(), 1);
}
