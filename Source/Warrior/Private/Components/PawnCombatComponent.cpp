// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PawnCombatComponent.h"

#include "Components/BoxComponent.h"
#include "Item/WarriorWeaponBase.h"


// Sets default values for this component's properties
UPawnCombatComponent::UPawnCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTag, AWarriorWeaponBase* InWeapon,
                                                 const bool bRegisterAsEquippedWeapon)
{
	if (CharacterCarriedWeapons.Contains(InWeaponTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has been registered"), *InWeaponTag.ToString())
	}
	if (InWeapon)
	{
		CharacterCarriedWeapons.Emplace(InWeaponTag, InWeapon);
		if (bRegisterAsEquippedWeapon)
		{
			CurrentEquippedWeaponTag = InWeaponTag;
		}
		InWeapon->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
		InWeapon->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
	}
}

AWarriorWeaponBase* UPawnCombatComponent::GetCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	if (CharacterCarriedWeapons.Contains(InWeaponTag))
	{
		AWarriorWeaponBase* CarriedWeapon = CharacterCarriedWeapons[InWeaponTag];
		return CarriedWeapon;
	}
	return  nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCurrentEquippedWeapon() const

{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return  nullptr;
	}
	return GetCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, const EToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
	{
		ToggleCurrentEquippedWeaponCollision(bShouldEnable);
	}
	else
	{
		ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);
	}
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* HitActor)
{
}

void UPawnCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable)
{
	const AWarriorWeaponBase* WeaponToToggle = GetCurrentEquippedWeapon();
	check(WeaponToToggle);
	if (bShouldEnable == true)
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlappedActors.Empty();
	}
}

void UPawnCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	
} 
