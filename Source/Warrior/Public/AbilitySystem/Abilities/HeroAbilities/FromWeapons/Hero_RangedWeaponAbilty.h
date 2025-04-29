// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Hero_RangedWeaponAbilty.generated.h"

class AWarriorHeroWeapon;
class AWarriorWeaponBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHero_RangedWeaponAbilty : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

public:

	UHero_RangedWeaponAbilty();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWarriorHeroWeapon* GetAssociatedWeapon();

protected:
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Ranged")
	void StartRangedWeaponTargeting( FVector& StartTraceLoc,  FVector& HitLoc);

	virtual void ProcessWeaponHit(const FHitResult& Hit);
	void PerformWeaponTrace(FVector StartTrace, FVector EndTrace, TArray<FHitResult>& OutHits);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TraceRadius = 0.0f;
	
};
