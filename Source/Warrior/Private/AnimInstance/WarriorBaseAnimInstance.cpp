// Fill out your copyright notice in the Description page of Project Settings.


#include "Warrior/Public/AnimInstance/WarriorBaseAnimInstance.h"

#include "WarriorFunctionLibrary.h"


bool UWarriorBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck) const
{
	if (APawn* OwningPawn =	TryGetPawnOwner())
	{
		return	UWarriorFunctionLibrary::NativeDoesActorHasTag(OwningPawn, TagToCheck);
	}
	return false;
}
