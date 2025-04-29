// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UIComponents/HeroUIComponent.h"
#include "WarriorWidgetBase.generated.h"

class UEnemyUIComponent;
/**
 * 
 */
class UHeroUIComponent;
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnOwningHeroUIComponentInitialized(UHeroUIComponent* OwingHeroUIComponent);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnOwningEnemyUIComponentInitialized(UEnemyUIComponent* OwningEnemyUIComponent);
	
public:

	UFUNCTION(BlueprintCallable,Category="Widget")
	void InitEnemyCreatedWidget(AActor* OwningEnemyActor);

	
};
