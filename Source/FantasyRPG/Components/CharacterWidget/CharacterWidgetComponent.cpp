#include "CharacterWidgetComponent.h"
#include "Actors/Characters/PlayerCharacter/PlayerCharacter.h"
#include "Actors/Controllers/PlayerController/RPGPlayerController.h"

#include "Widgets/CharacterWidget.h"

#include "Single/GameInstance/FRGameInstance.h"
#include "Single/PlayerManager/PlayerManager.h"

UCharacterWidgetComponent::UCharacterWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// 위젯 표시 최대 거리 설정
	MaxDrawableDistance = 2000.0f;
}

void UCharacterWidgetComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DistanceToggle();
}


void UCharacterWidgetComponent::DistanceToggle()
{
	if (!IsValid(PlayerCamera))
	{
		UFRGameInstance* gameInst = Cast<UFRGameInstance>(GetWorld()->GetGameInstance());

		if (!IsValid(gameInst)) return;

		UPlayerManager* playerManager = gameInst->GetManagerClass<UPlayerManager>();

		PlayerCamera = Cast<APlayerCharacter>(playerManager->GetPlayerController()->GetPawn())->GetCameraComponent();
		return;
	}

	// 카메라와 적 캐릭터의 거리를 구합니다.
	float distance = FVector::Distance(
		PlayerCamera->GetComponentLocation(),
		GetOwner()->GetActorLocation());

	// 거리에 따라 위젯을 표시합니다.
	bVisible = (distance <= MaxDrawableDistance);
}

void UCharacterWidgetComponent::InitializeCharaterWidget(
	TSubclassOf<class UCharacterWidget> widgetClass,
	FVector2D drawSize,
	EWidgetSpace newWidgetSpace)
{
	SetWidgetClass(widgetClass);
	SetDrawSize(drawSize);
	SetWidgetSpace(newWidgetSpace);
}

void UCharacterWidgetComponent::SetWidgetHeight(float widgetHeight)
{
	SetRelativeLocation(FVector::UpVector * widgetHeight);
}
