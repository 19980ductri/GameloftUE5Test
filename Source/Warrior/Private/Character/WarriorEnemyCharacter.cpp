
#include "Character/WarriorEnemyCharacter.h"

#include "WarriorFunctionLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/EnemyCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/UIComponents/EnemyUIComponent.h"
#include "DataAssets/DataAsset_EnemyStartupData.h"
#include "DataAssets/DataAsset_StartupDataBase.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/WarriorWidgetBase.h"

// Sets default values
AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 180.f, 0);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");	
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("UI Component");
	EnemyHealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Health Widget Component");
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	LeftHandBoxComponent = CreateDefaultSubobject<UBoxComponent>("Left Hand box Component");
	LeftHandBoxComponent->SetupAttachment(GetMesh());
	LeftHandBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);

	RightHandBoxComponent = CreateDefaultSubobject<UBoxComponent>("Right Hand box Component");
	RightHandBoxComponent->SetupAttachment(GetMesh());
	RightHandBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandBoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
} 

UPawnCombatComponent* AWarriorEnemyCharacter::GetCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APawn* HitPawn = Cast<APawn>(OtherActor);
	if (HitPawn != nullptr)
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
}


void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject());
	if (HealthWidget)
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}

void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartupData();
}

void AWarriorEnemyCharacter::InitEnemyStartupData()
{
	if (StartUpAbilityData.IsNull())
	{
		return;
	}

	const FStreamableDelegate StreamableDelegate = FStreamableDelegate::CreateLambda([this] ()
		{
			if (const UDataAsset_EnemyStartupData* LoadedData = CastChecked<UDataAsset_EnemyStartupData>(StartUpAbilityData.Get()))
			{
				WarriorAbilitySystemComponent->GrantDefaultAbilitiesData(LoadedData,1);
				WarriorAbilitySystemComponent->GrantAbilities(LoadedData->GetEnemyCombatAbilities(),1);
				//WarriorAbilitySystemComponent->GrantAbilities(LoadedData->GetEnemyCombatAbilities(),1);
			}
		});
	
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		StartUpAbilityData.ToSoftObjectPath(), StreamableDelegate);
	
}

#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollsionBoxAttachBoneName))
	{
		LeftHandBoxComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollsionBoxAttachBoneName);
	}
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollsionBoxAttachBoneName))
	{
		RightHandBoxComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollsionBoxAttachBoneName);
	}
}
#endif

