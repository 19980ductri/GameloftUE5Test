// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroAbilities/FromWeapons/Hero_RangedWeaponAbilty.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Character/WarriorHeroCharacter.h"
#include "Components/HeroCombatComponent.h"
#include "Item/WarriorHeroWeapon.h"

UHero_RangedWeaponAbilty::UHero_RangedWeaponAbilty()
{
}

bool UHero_RangedWeaponAbilty::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (bResult)
	{
		// Check if we have a valid actor info
		if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid actor info or avatar actor"));
			return false;
		}
        
		// Try to get the hero character directly
		const AWarriorHeroCharacter* HeroCharacter = Cast<AWarriorHeroCharacter>(ActorInfo->AvatarActor.Get());
		if (!HeroCharacter)
		{
			UE_LOG(LogTemp, Error, TEXT("Avatar actor is not a hero character"));
			return false;
		}
        
		// Try to get the combat component
		UHeroCombatComponent* CombatComponent = HeroCharacter->GetHeroCombatComponent();
		if (!CombatComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("Hero character has no combat component"));
			return false;
		}
        
		// Check if we have a weapon
		AWarriorHeroWeapon* Weapon = CombatComponent->GetHeroCurrentEquippedWeapon();
		if (!Weapon)
		{
			UE_LOG(LogTemp, Error, TEXT("No weapon equipped"));
			return false;
		}
	}
	return bResult;
}

void UHero_RangedWeaponAbilty::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHero_RangedWeaponAbilty::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHero_RangedWeaponAbilty::StartRangedWeaponTargeting(FVector& StartTraceLoc, FVector& HitLoc)
{
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(ASC);

	// Get the character and calculate trace points
	if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		// Get the player controller for camera view
		APlayerController* PC = Character->GetController<APlayerController>();
		if (!PC)
		{
			return;
		}

		// Get camera view point
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		// Calculate trace end point (center of screen)
		FVector TraceDirection = CameraRotation.Vector();
		FVector EndTrace = CameraLocation + (TraceDirection * MaxRange);

		// Perform the trace
		TArray<FHitResult> HitResults;
		PerformWeaponTrace(CameraLocation, EndTrace, HitResults);

		// Process all hits
		for (const FHitResult& Hit : HitResults)
		{
			ProcessWeaponHit(Hit);
		}
		HitLoc = HitResults[0].Location;
		StartTraceLoc = CameraLocation;
	}
}
void UHero_RangedWeaponAbilty::ProcessWeaponHit(const FHitResult& Hit)
{
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	APawn* OwningPawn = Cast<APawn>(AvatarActor);
	if (!OwningPawn)
	{
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		return;
	}

	// Check if we hit a valid target (another pawn that is hostile)
	APawn* HitPawn = Cast<APawn>(HitActor);
	if (HitPawn && UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
	{
		// Create damage effect
		if (DamageEffectClass)
		{
			// Get weapon damage from the associated weapon
			AWarriorHeroWeapon* Weapon = GetAssociatedWeapon();
			if (Weapon)
			{
				// Create a gameplay effect spec for damage
				float WeaponDamage = Weapon->WeaponData.WeaponBaseDamage.GetValueAtLevel(GetAbilityLevel());
				UE_LOG(LogTemp, Warning, TEXT("Weapon Damage: %f"), WeaponDamage)
				FGameplayTag EmptyTag;
				FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeHeroDamageEffectSpecHandle(DamageEffectClass, WeaponDamage, EmptyTag, 0);

				// Apply the effect to the target
				FActiveGameplayEffectHandle ActiveGEHandle = NativeApplyEffectSpecHandleToTarget(HitActor, DamageEffectSpecHandle);

				// Send hit react event if damage was applied successfully
				if (ActiveGEHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData EventData;
					EventData.Instigator = OwningPawn;
					EventData.Target = HitActor;
					
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, WarriorGameplayTags::Shared_Event_HitReact, EventData);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Null wepaon"));
				
			}
		}
	}
}

void UHero_RangedWeaponAbilty::PerformWeaponTrace(FVector StartTrace, FVector EndTrace, TArray<FHitResult>& OutHits)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	// Setup collision parameters
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	// Add any actors attached to the avatar to ignore list
	TArray<AActor*> AttachedActors;
	AvatarActor->GetAttachedActors(AttachedActors);
	QueryParams.AddIgnoredActors(AttachedActors);
	
	// Determine which trace to use based on radius
	if (TraceRadius > 0.0f)
	{
		// Sphere trace for radius > 0
		UWorld* World = GetWorld();
		if (World)
		{
			World->SweepMultiByChannel(
				OutHits,
				StartTrace,
				EndTrace,
				FQuat::Identity,
				ECC_Pawn,
				FCollisionShape::MakeSphere(TraceRadius),
				QueryParams);
		}
	}
	else
	{
		// Line trace for radius = 0
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceMultiByChannel(
				OutHits,
				StartTrace,
				EndTrace,
				ECC_Pawn,
				QueryParams);
		}
	}

	/*
	// Debug visualization
#if ENABLE_DRAW_DEBUG
	if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		if (Character->IsLocallyControlled())
		{
			if (TraceRadius > 0.0f)
			{
				DrawDebugSphere(GetWorld(), EndTrace, TraceRadius, 16, FColor::Red, false, 2.0f);
				DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 2.0f);
			}
			else
			{
				DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, 2.0f);
			}
		}
	}
#endif
*/

}

AWarriorHeroWeapon* UHero_RangedWeaponAbilty::GetAssociatedWeapon()
{
	return GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeapon();
}
