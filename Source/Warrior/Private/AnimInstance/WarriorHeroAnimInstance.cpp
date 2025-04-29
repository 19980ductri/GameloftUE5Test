// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WarriorHeroAnimInstance.h"

#include "Character/WarriorHeroCharacter.h"

void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwningCharacter)
	{
		OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
	}
}

void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (bHasAcceleration)
	{
		IdleElapsedTime = 0;
		bShouldEnterRelaxState = false;
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		if (IdleElapsedTime >= EnterRelaxThreshold)
		{
			bShouldEnterRelaxState = true;
		}
	}
	CalculateAO_Pitch();
}
void UWarriorHeroAnimInstance::CalculateAO_Pitch()
{
	if (OwningHeroCharacter)
	{
		AO_Pitch = OwningHeroCharacter->GetBaseAimRotation().Pitch;
	    
		if (AO_Pitch > 90.f)
		{
			// map pitch from [270, 360) to [-90, 0)
			FVector2D InRange(270.f, 360.f);
			FVector2D OutRange(-90.f, 0.f);
			AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
		}
	}
}
