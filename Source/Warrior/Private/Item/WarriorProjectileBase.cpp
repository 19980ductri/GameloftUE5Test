#include "Item/WarriorProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
AWarriorProjectileBase::AWarriorProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);
	
	ProjectileNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f,0.f,0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;
	InitialLifeSpan = 4.f;
}
void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	}
	else
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	}
	
}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{	
		BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

		APawn* HitPawn = Cast<APawn>(OtherActor);

		if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
		{
			
			Destroy();
			return;
		}

		bool bIsValidBlock = false;

		const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHasTag(HitPawn,WarriorGameplayTags::Player_Status_Blocking);

		if (bIsPlayerBlocking)
		{
			bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this,HitPawn);
		}

		FGameplayEventData Data;
		Data.Instigator = this;
		Data.Target = HitPawn;

		if (bIsValidBlock)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				HitPawn,
				WarriorGameplayTags::Player_Event_SuccessfulBlock,
				Data
			);
		}
		else
		{
			//Apply projectile damage
			HandpleApplyProjectileDamage(HitPawn, Data);
		}

		Destroy();
	}
}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}
	OverlappedActors.AddUnique(OtherActor);
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = GetInstigator();
		Data.Target = HitPawn;
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{
			HandpleApplyProjectileDamage(HitPawn, Data);
		}
	}
}

void AWarriorProjectileBase::HandpleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	const bool WasApplied = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(
		GetInstigator(),
		InHitPawn,
		ProjectileDamageEffectSpecHandle);
	
	if (WasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WarriorGameplayTags::Shared_Event_HitReact,
			InPayload);
	}
} 
