// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset_StartupDataBase.h"
#include "WarriorStructTypes.h"
#include "DataAsset_HeroStartupData.generated.h"


UCLASS()
class WARRIOR_API UDataAsset_HeroStartupData : public UDataAsset_StartupDataBase
{
	GENERATED_BODY()

public:

	TArray<FWarriorAbilitySet> GetHeroStartupAbilitiesToGrant() const;
	
private:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= StartupData, meta=(AllowPrivateAccess = "true"))
	TArray<FWarriorAbilitySet> StartupHeroAbilitySets;
};