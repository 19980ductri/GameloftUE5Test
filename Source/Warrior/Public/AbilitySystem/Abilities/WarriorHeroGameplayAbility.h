// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorGameplayAbility.h"
#include "WarriorHeroGameplayAbility.generated.h"

class UHeroCombatComponent;
class AWarriorHeroController;
class AWarriorHeroCharacter;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Ability")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Ability")
	AWarriorHeroController* GetHeroControllerFromActorInfo();

	UFUNCTION(BlueprintPure, Category = "Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	UFUNCTION(BlueprintPure, Category= "Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage
	                                                         ,FGameplayTag InCurrentAttackTag, int32 InUsedComboCount);

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool GetAbilityRemainCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainCooldownTime);
	
private:

	TWeakObjectPtr<AWarriorHeroCharacter> CachedHeroCharacter;
	TWeakObjectPtr<AWarriorHeroController> CachedHeroController;
};
