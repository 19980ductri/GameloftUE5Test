// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/DataAsset_HeroStartupData.h"


TArray<FWarriorAbilitySet> UDataAsset_HeroStartupData::GetHeroStartupAbilitiesToGrant() const
{
	return StartupHeroAbilitySets;
}
