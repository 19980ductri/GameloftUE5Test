// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/WarriorWidgetBase.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UIComponents/HeroUIComponent.h"

void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (const IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent =	PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}
	
}

void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (const IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();
		check(EnemyUIComponent)
		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
	
}

