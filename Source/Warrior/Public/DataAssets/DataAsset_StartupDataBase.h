// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartupDataBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;
class UWarriorGameplayAbility;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class WARRIOR_API UDataAsset_StartupDataBase : public UDataAsset
{
	GENERATED_BODY()

public:

	//virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InWarriorASCToGive, int32 ApplyLevel = 1);

	FORCEINLINE TArray<TSubclassOf<UWarriorGameplayAbility>> GetActivateGivenAbilities() const { return ActivateGivenAbilities; }
	FORCEINLINE TArray<TSubclassOf<UWarriorGameplayAbility>> GetReactiveGivenAbilities() const { return ReactivateGivenAbilities; }
	FORCEINLINE TArray<TSubclassOf<UGameplayEffect>> GetStartupGameplayEffects() const { return StartupGameplayEffects; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category= StartupData)
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category= StartupData)
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactivateGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category= StartupData)
	TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffects;
};	
