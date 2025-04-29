// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroAbilities/HeroGameplayAbility_TargetLock.h"

#include "EnhancedInputSubsystems.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Character/WarriorHeroCharacter.h"
#include "Components/SizeBox.h"
#include "Controllers/WarriorHeroController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/Widget/WarriorWidgetBase.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMappingContext();
	InitTargetLockMovement();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUpTargetLockAbility();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor ||
		UWarriorFunctionLibrary::NativeDoesActorHasTag(CurrentLockedActor, WarriorGameplayTags::Shared_Status_Dead) ||
		UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();
		return;
	}
	SetTargetLockWidgetPosition();
	const bool bShouldOverrideRotation =
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Rolling) &&
			!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Blocking);
	if (bShouldOverrideRotation == true)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			GetHeroCharacterFromActorInfo()->GetActorLocation(),
			CurrentLockedActor->GetActorLocation()
		);

		LookAtRot -= FRotator(CameraOffSetDistance, 0.f,0.f);
		const FRotator CurrentControllerRot = GetHeroControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControllerRot, LookAtRot, DeltaTime, TargetLockInterpSpeed);

		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0));
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0, TargetRot.Yaw, 0));
	}
	
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	CurrentLockedActor = GetNearestFromAvailableActors(AvailableActorsToLock);
	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();	
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
	//DrawDebugSphere(GetWorld(), CurrentLockedActor->GetActorLocation(), 100, 20, FColor::Yellow, false, 10.f);	
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();

	TArray<FHitResult> BoxTraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance,
		TraceBoxSize/2.f,
		GetHeroCharacterFromActorInfo()->GetActorLocation().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
		);	
	
	for (const FHitResult& HitResult : BoxTraceHits)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			if (HitActor == GetHeroCharacterFromActorInfo()) continue;
			AvailableActorsToLock.AddUnique(HitActor);
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableActors, ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		return;
	}

	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentLockedActorNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (AvailableActor == nullptr || AvailableActor == CurrentLockedActor) continue;
		
		const FVector PlayerToAvailableActorNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentLockedActorNormalized, PlayerToAvailableActorNormalized);

		if (CrossResult.Z  > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		check(TargetLockWidgetClass);
		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);
		check(DrawnTargetLockWidget);
		DrawnTargetLockWidget->AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (DrawnTargetLockWidget == nullptr || CurrentLockedActor == nullptr )
	{
		CancelTargetLockAbility();
		return;
	}
	//Find the screen 
	FVector2d ScreenPos;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetHeroControllerFromActorInfo(),
		CurrentLockedActor->GetActorLocation(),
		ScreenPos,
		true);

	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
		[this](UWidget* FoundWidget)
		{
			if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
			{
				TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
				TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
			}	
		});
	}
	ScreenPos -= (TargetLockWidgetSize / 2.f);
	
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPos, false);
	
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed =	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
	
}	

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed > 0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	Subsystem->AddMappingContext(TargetLockMappingContext, 3);
	
	//UE_LOG(LogTemp, Warning, TEXT("Add target lock mapping context"))
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetHeroControllerFromActorInfo())
	{
		return;
	}
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);
	Subsystem->RemoveMappingContext(TargetLockMappingContext);
	//UE_LOG(LogTemp, Warning, TEXT("Remove target lock mapping context"))
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::CleanUpTargetLockAbility()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	
	GetAvailableActorsToLock();
	TArray<AActor*> LeftActors;
	TArray<AActor*> RightActors;
	
	AActor* NewTargetToLock = nullptr;
	GetAvailableActorsAroundTarget(LeftActors, RightActors);

	if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestFromAvailableActors(LeftActors);
	}
	else if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Right)
	{
		NewTargetToLock = GetNearestFromAvailableActors(RightActors);
	}
	
	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
	}
	
}
