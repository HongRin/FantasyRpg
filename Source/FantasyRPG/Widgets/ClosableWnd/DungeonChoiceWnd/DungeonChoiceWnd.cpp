#include "DungeonChoiceWnd.h"

#include "Actors/Characters/PlayerCharacter/PlayerCharacter.h"
#include "Actors/Characters/MercenaryCharacter/MercenaryCharacter.h"
#include "Actors/Controllers/PlayerController/RPGPlayerController.h"

#include "Components/MercenaryWidget/MercenaryState/MercenaryStateComponent.h"

#include "Single/GameInstance/FRGameInstance.h"
#include "Single/PlayerManager/PlayerManager.h"

#include "Structures/DCWInfo/DCWInfo.h"

#include "Widgets/ClosableWnd/DungeonChoiceWnd/DungeonRow/DungeonRow.h"
#include "Widgets/ClosableWnd/DungeonChoiceWnd/ParticipateRow/ParticipateRow.h"

#include "Components/ScrollBox.h"
#include "Components/Button.h"


UDungeonChoiceWnd::UDungeonChoiceWnd(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_DCW_INFO(
		TEXT("DataTable'/Game/Resources/DataTables/DT_DCWInfo.DT_DCWInfo'"));
	if (DT_DCW_INFO.Succeeded()) DT_DCWInfo = DT_DCW_INFO.Object;
	else UE_LOG(LogTemp, Error, TEXT("UDungeonChoiceWnd.cpp::%d::LINE:: DT_DCW_INFO is not loaded!"), __LINE__);

	static ConstructorHelpers::FClassFinder<UDungeonRow> BP_DUNGEON_ROW(
		TEXT("WidgetBlueprint'/Game/Resources/Blueprints/Widgets/ClosableWnd/ClosableDungeonWnd/BP_DungeonRow.BP_DungeonRow_C'"));
	if (BP_DUNGEON_ROW.Succeeded()) BP_DungeonRow = BP_DUNGEON_ROW.Class;
	else UE_LOG(LogTemp, Error, TEXT("UDungeonChoiceWnd.cpp::%d::LINE:: BP_DUNGEON_ROW is not loaded!"), __LINE__);

	static ConstructorHelpers::FClassFinder<UParticipateRow> BP_PARTICIPATE_ROW(
		TEXT("WidgetBlueprint'/Game/Resources/Blueprints/Widgets/ClosableWnd/ClosableDungeonWnd/BP_ParticipateRow.BP_ParticipateRow_C'"));
	if (BP_PARTICIPATE_ROW.Succeeded()) BP_ParticipateRow = BP_PARTICIPATE_ROW.Class;
	else UE_LOG(LogTemp, Error, TEXT("UDungeonChoiceWnd.cpp::%d::LINE:: BP_PARTICIPATE_ROW is not loaded!"), __LINE__);
}

void UDungeonChoiceWnd::NativeConstruct()
{
	Super::NativeConstruct();

	ScrollBox_DungeonList = Cast<UScrollBox>(GetWidgetFromName(TEXT("ScrollBox_DungeonList")));

	ScrollBox_Participate = Cast<UScrollBox>(GetWidgetFromName(TEXT("ScrollBox_Participate")));

	Button_Cancel = Cast<UButton>(GetWidgetFromName(TEXT("Button_Cancel")));

	UpdateWndSize(1920.0f, 1080.0f);

	Button_Cancel->OnClicked.AddDynamic(this, &UDungeonChoiceWnd::QuitDundeonWnd);

	MercenaryState = Cast<APlayerCharacter>(GetManager(UPlayerManager)->
		GetPlayerController()->GetPawn())->GetMercenaryState();

	UpdateParticipate();
}

void UDungeonChoiceWnd::UpdateParticipate()
{
	for (int i = 0; i < MercenaryState->GetMercenaryActors().Num(); ++i)
	{
		UParticipateRow* participateRow = CreateWidget<UParticipateRow>(this, BP_ParticipateRow);

		participateRow->UpdatePartcipateRow(MercenaryState->GetMercenaryActors()[i]->GetMercenaryInfo());

		participateRow->SetDungeonChoiceWnd(this);

		ScrollBox_Participate->AddChild(participateRow);
	}
}

void UDungeonChoiceWnd::QuitDundeonWnd()
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetManager(UPlayerManager)->
		GetPlayerController()->GetPawn());

	playerCharacter->AllowMove();

	playerCharacter->GetPlayerController()->SetInputMode(FInputModeGameOnly());

	OnCloseButtonClicked();
}

void UDungeonChoiceWnd::InitializeDCW(FName dcwCode)
{
	FString contextString;
	FDCWInfo* dcwInfo = DT_DCWInfo->FindRow<FDCWInfo>(dcwCode, contextString);

	for (int i = 0; i < dcwInfo->DungeonRowCode.Num(); ++i)
	{
		UDungeonRow* dungeonRow = CreateWidget<UDungeonRow>(this, BP_DungeonRow);

		UE_LOG(LogTemp, Warning, TEXT("dcwInfo->DungeonRowCode[%d] :: %s"), i, *dcwInfo->DungeonRowCode[i].ToString());

		dungeonRow->InitializeDungeonRow(dcwInfo->DungeonRowCode[i]);

		ScrollBox_DungeonList->AddChild(dungeonRow);
	}
}
