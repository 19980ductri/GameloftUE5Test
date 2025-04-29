// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtentionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WARRIOR_API UPawnExtentionComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPawnExtentionComponentBase();


protected:
	template <class T>
	T* GetOwningPawn()const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value,"'T' Template parameter must be derived from 'APawn'");
		return CastChecked<T>(GetOwner());
	}

	APawn* GetOwningPawn()const
	{
		return GetOwningPawn<APawn>();
	}

	template <class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value,"'T' Template parameter must be derived from 'AController'");
		return GetOwningPawn<APawn>()->GetController();
	}

	AController* GetOwningController()const
	{
		return GetOwningController<AController>();
	}
};
