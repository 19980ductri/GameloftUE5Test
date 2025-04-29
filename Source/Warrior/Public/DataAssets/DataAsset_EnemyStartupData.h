// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/DataAsset_StartupDataBase.h"
#include "DataAsset_EnemyStartupData.generated.h"

class UWarriorEnemyGameplayAbility;
/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_EnemyStartupData : public UDataAsset_StartupDataBase
{
	GENERATED_BODY()

public:

	FORCEINLINE	TArray<TSubclassOf<UWarriorGameplayAbility>> GetEnemyCombatAbilities() const {return EnemyCombatAbilities;}
	
private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= StartupData, meta=(AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UWarriorGameplayAbility>> EnemyCombatAbilities;
};
