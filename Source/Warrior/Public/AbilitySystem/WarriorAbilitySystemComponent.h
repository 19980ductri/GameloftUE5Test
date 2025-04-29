// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

class UDataAsset_HeroStartupData;
class UDataAsset_StartupDataBase;
class UWarriorGameplayAbility;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UWarriorAbilitySystemComponent();

	void GrantDefaultAbilitiesData(const UDataAsset_StartupDataBase* StartUpAbilitiesData, int32 Level);

	void GrantStartupAbilitySets(const TArray<FWarriorAbilitySet>& HeroStartupAbilitySet, int32 Level);

	void OnAbilityInputPressed(const FGameplayTag& InputTag);
	void OnAbilityInputReleased(const FGameplayTag& InputTag);
	
	FGameplayAbilitySpecHandle GrantAbilityWithAbilityData(int32 InLevel, const FWarriorAbilitySet& AbilitySet);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	TArray<FGameplayAbilitySpecHandle> GrandWeaponAbilities(const TArray<FWarriorAbilitySet>& InDefaultWeaponAbilities, const TArray<FSpecialHeroAbilitySet>& InWeaponAbilities, int32
	                                                        InLevel = 1);
	
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
	
	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& AbilitiesToGrant, int32 Level = 1);

	void ApplyGameplayEffect(TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffect, int Level);

	
	
protected:
	
	
};
