// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WarriorHeroWeapon.h"

#include "Components/BoxComponent.h"


// Sets default values
AWarriorHeroWeapon::AWarriorHeroWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
}

void AWarriorHeroWeapon::AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandle)
{
	GrantedAbilitiesSpecHandles = InSpecHandle;
}

TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
	return GrantedAbilitiesSpecHandles;
}
