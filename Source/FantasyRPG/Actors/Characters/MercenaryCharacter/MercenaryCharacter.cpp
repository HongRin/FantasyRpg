#include "MercenaryCharacter.h"

#include "Engine/Engine.h"


#include "Actors/Characters/PlayerCharacter/PlayerCharacter.h"
#include "Actors/Controllers/MercenaryController/MercenaryController.h"
#include "Actors/Controllers/PlayerController/RPGPlayerController.h"

#include "Components/PlayerDetector/PlayerDetectorComponent.h"
#include "Components/MercenaryAttack/MercenaryAttackComponent.h"
#include "Components/HealerBehavior/HealerBehaviorComponent.h"

#include "AnimInstances/MercenaryCharacter/MercenaryAnimInstance.h"

#include "Single/GameInstance/FRGameInstance.h"
#include "Single/PlayerManager/PlayerManager.h"

#include "BrainComponent.h"

#include "Widgets/HpableCharacterWidget/HpableCharacterWidget.h"
#include "Widgets/HpableCharacterWidget/PlayerCharacterWidget/PlayerCharacterWidget.h"
#include "Widgets/ClosableWnd/MercenaryHpWnd/MercenaryHpWnd.h"

AMercenaryCharacter::AMercenaryCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MERCENARY_INFO(
		TEXT("DataTable'/Game/Resources/DataTables/DT_MercenaryInfo.DT_MercenaryInfo'"));
	if (DT_MERCENARY_INFO.Succeeded()) MercenaryDatatable = DT_MERCENARY_INFO.Object;
	else UE_LOG(LogTemp, Error, TEXT("AMercenaryCharacter.cpp :: %d :: LINE :: DT_MERCENARY_INFO is not loaded !"), __LINE__);

	// Component Initialization
	InitializeComponent();

	SetGenericTeamId(TEAM_MERCENRAY);
}
 
void AMercenaryCharacter::BeginPlay()
{
	Super::BeginPlay();

	GameInst = Cast<UFRGameInstance>(GetGameInstance());

	PlayerCharacter = Cast<APlayerCharacter>(GetManager(UPlayerManager)->GetPlayerController()->GetPawn());

	// Set up the Monster Info
	InitializeMercenaryDataConstructTime();

	Tags.Add(TEXT("Mercenary"));

	LoadMercenaryInfo();
}

void AMercenaryCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetMoveSpeed();
}

void AMercenaryCharacter::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::OnTakeDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	if (PlayerCharacter->GetPlayerController()->GetPlayerCharacterWidgetInstance()->GetMercenaryHpWnd()->OnTakeDamageEvent.IsBound())
		PlayerCharacter->GetPlayerController()->GetPlayerCharacterWidgetInstance()->GetMercenaryHpWnd()->OnTakeDamageEvent.Broadcast();
}

void AMercenaryCharacter::OnCharacterDie()
{
	Super::OnCharacterDie();

	FString reason;
	Cast<AMercenaryController>(GetController())->GetBrainComponent()->StopLogic(reason);

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		timerHandle,
		[this]() { Destroy(); },
		3.0f,
		false);
}

void AMercenaryCharacter::InitializeComponent()
{
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_HAIR"));
	HairMesh->SetupAttachment(GetMesh(), TEXT("Hair_Socket"));

	LeftWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_LEFT_WEAPON"));
	LeftWeaponMesh->SetupAttachment(GetMesh(), TEXT("R_WeaponSocket"));

	RightWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SK_RIGHT_WEAPON"));
	RightWeaponMesh->SetupAttachment(GetMesh(), TEXT("L_WeaponSocket"));

	PlayerDetector = CreateDefaultSubobject<UPlayerDetectorComponent>(TEXT("PLAYER_DETECTOR"));
	PlayerDetector->SetupAttachment(GetRootComponent());

	MercenaryAttack = CreateDefaultSubobject<UMercenaryAttackComponent>(TEXT("ATTACK_COMPONENT"));
	HealerBehavior = CreateDefaultSubobject<UHealerBehaviorComponent>(TEXT("HEALER_COMPONET"));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("MercenaryCollision"));
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
}

void AMercenaryCharacter::InitializeSkeletalMeshComponent()
{
	// Monster SkeletalMesh synchronous load
	auto mercenaryMesh = Cast<USkeletalMesh>(
		GameInst->GetStreamableManager()->LoadSynchronous(MercenaryInfo.SkeletalMeshPath));

	if (IsValid(mercenaryMesh))
	{
		// Apply the loaded SkeletalMesh
		GetMesh()->SetSkeletalMesh(mercenaryMesh);

		// Set up the Location & Rotation
		GetMesh()->SetRelativeLocationAndRotation(
			FVector(0.0f, 0.0f, -(GetCapsuleComponent()->GetScaledCapsuleHalfHeight())),
			FRotator(0.0f, -90.0f, 0.0f));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("AMonsterCharacter.cpp :: %d LINE :: mercenaryMesh is not loaded!"), __LINE__);
	
	GetMesh()->SetAnimClass(MercenaryInfo.AnimInstanceClassPath);
	AIControllerClass = MercenaryInfo.MercenaryControllerClassPath;
}

void AMercenaryCharacter::SetMoveSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed - 50.0f;
}

void AMercenaryCharacter::LoadMercenaryInfo()
{
	FString contextString;
	FMercenaryInfo* mercenaryInfo = MercenaryDatatable->FindRow<FMercenaryInfo>(MercenaryCode, contextString);

	UPlayerManager* playerManager = GetManager(UPlayerManager);
	if (playerManager->GetMercenaryInfo().Num() != 0)
	{
		for (int i = 0; i < playerManager->GetMercenaryInfo().Num(); ++i)
		{
			if (mercenaryInfo->MercenaryCode == playerManager->GetMercenaryInfo()[i].MercenaryCode)
			{
				MercenaryInfo = playerManager->GetMercenaryInfo()[i];

				Hp = MaxHp = MercenaryInfo.MaxHp;
			}
		}
	}
}

void AMercenaryCharacter::InitializeMercenaryDataConstructTime()
{
	// Saves the Infomation that matches MonsterCode 
	FString contextString;
	FMercenaryInfo* mercenaryInfo = MercenaryDatatable->FindRow<FMercenaryInfo>(MercenaryCode, contextString);

	// Check for information found
	if (mercenaryInfo == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AMonsterCharacter.cpp :: %d LINE :: mercenaryInfo is not loaded!"), __LINE__);
		return;
	}

	MercenaryInfo = (*mercenaryInfo);

	// Set up the Hp
	Hp = MaxHp = MercenaryInfo.MaxHp;

	// SkeletalMesh Initialization
	InitializeSkeletalMeshComponent();
}

void AMercenaryCharacter::MercenaryDash(FVector direction, float power)
{
	GetCharacterMovement()->AddImpulse(direction * power, true);
}

bool AMercenaryCharacter::IsMercenaryMovable() const
{
	return !(GetMercenaryAttack()->IsMercenaryAttacking());
}

bool AMercenaryCharacter::IsHealerMovable() const
{
	return !(GetHealerBehavior()->IsHealerSkillNone());
}