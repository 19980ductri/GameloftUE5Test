// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WarriorHeroLinkedAnimLayer.h"

#include "AnimInstance/WarriorHeroAnimInstance.h"


UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	return	Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
