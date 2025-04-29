// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WarriorHeroController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Character/WarriorBaseCharacter.h"
#include "Components/Input/WarriorInputComponent.h"
#include "GameFramework/Character.h"


void AWarriorHeroController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* InputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(InputLocalPlayerSubsystem);

	InputLocalPlayerSubsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	
	UWarriorInputComponent* WarriorInputComponent =	CastChecked<UWarriorInputComponent>(InputComponent);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_Move,ETriggerEvent::Triggered,this,&ThisClass::MoveCharacter);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_Look,ETriggerEvent::Triggered,this,&ThisClass::Look);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_SwitchTarget,ETriggerEvent::Triggered,this,&ThisClass::Input_SwitchTargetTriggered);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_SwitchTarget,ETriggerEvent::Completed,this,&ThisClass::Input_SwitchTargetCompleted);
	
	//for input ability
	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset, this,
		&ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);


	HeroTeamID = FGenericTeamId(0);
}

void AWarriorHeroController::MoveCharacter(const FInputActionValue& InputActionValue)
{
	// Example: Use Value.Get<float>() to retrieve the float value
	const FVector2d MovementVector = InputActionValue.Get<FVector2d>();
	const FRotator MovementRotation(0, GetControlRotation().Yaw, 0);

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		GetCharacter()->AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		GetCharacter()->AddMovementInput(RightDirection, MovementVector.X);
	}

}

void AWarriorHeroController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2d& LookDirection = InputActionValue.Get<FVector2d>();
	
	if (LookDirection.X != 0.f)
	{
		AddYawInput(LookDirection.X);
	}
	if (LookDirection.Y != 0.f)
	{
		AddPitchInput(LookDirection.Y);
	}

	

}


void AWarriorHeroController::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection = InputActionValue.Get<FVector2d>();
	//UE_LOG(LogTemp, Warning, TEXT("AWarriorHeroController::Look"));
	//GEngine->AddOnScreenDebugMessage(1,2,FColor::Red, FString::Printf(TEXT("%f"), SwitchDirection.X));
}

void AWarriorHeroController::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	const FGameplayEventData EventData;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetCharacter(),
		SwitchDirection.X > 0.f ? WarriorGameplayTags::Player_Event_SwitchTarget_Right : WarriorGameplayTags::Player_Event_SwitchTarget_Left,
		EventData);
	
}

void AWarriorHeroController::Input_AbilityInputPressed(FGameplayTag InputTag)
{	
	GetWarriorAbilitySystemComponent()->OnAbilityInputPressed(InputTag);
}

void AWarriorHeroController::Input_AbilityInputReleased(FGameplayTag InputTag)
{
	GetWarriorAbilitySystemComponent()->OnAbilityInputReleased(InputTag);
}

UWarriorAbilitySystemComponent* AWarriorHeroController::GetWarriorAbilitySystemComponent()
{
	if (WarriorAbilitySystemComponent == nullptr)
	{
		WarriorAbilitySystemComponent =	Cast<AWarriorBaseCharacter>(GetCharacter())->GetWarriorAbilitySystemComponent();
		return WarriorAbilitySystemComponent;
	}
	return WarriorAbilitySystemComponent;
}

FGenericTeamId AWarriorHeroController::GetGenericTeamId() const
{
	return HeroTeamID;
}