// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UIComponents/EnemyUIComponent.h"

#include "UI/Widget/WarriorWidgetBase.h"


void UEnemyUIComponent::RegisterEnemyDrawnWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidget()
{
	if (EnemyDrawnWidgets.IsEmpty())
	{
		return;
	}

	for (UWarriorWidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}		
	}
	EnemyDrawnWidgets.Empty();
}
