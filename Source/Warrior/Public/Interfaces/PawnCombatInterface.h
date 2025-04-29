// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnCombatInterface.generated.h"

class UPawnCombatComponent;
// This class does not need to be modified.
UINTERFACE()
class UPawnCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class WARRIOR_API IPawnCombatInterface
{
	GENERATED_BODY()
	
public:

	virtual UPawnCombatComponent* GetCombatComponent() const = 0;
	
};
