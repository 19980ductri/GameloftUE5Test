// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorCountDownAction.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorAscFromActor(AActor* InActor)
{
	check(InActor);
	
	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
	
}

float UWarriorFunctionLibrary::NativeGetAngleDifference(AActor* A, AActor* B, FVector& OutAForwardVector, FVector& OutAtoBNormalized )
{
	check(A && B);
	OutAForwardVector = A->GetActorForwardVector();
	OutAtoBNormalized = (B->GetActorLocation() - A->GetActorLocation()).GetSafeNormal();
	const float DotProduct = FVector::DotProduct(OutAForwardVector, OutAtoBNormalized);
	
	return UKismetMathLibrary::DegAcos(DotProduct);
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, const FGameplayTag InTag)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorAscFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(InTag) == false)
	{
		ASC->AddLooseGameplayTag(InTag);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayTagIfFound(AActor* InActor, const FGameplayTag InTag)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorAscFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(InTag) == true)
	{
		ASC->RemoveLooseGameplayTag(InTag);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHasTag(AActor* InActor, FGameplayTag InTag)
{
	const UWarriorAbilitySystemComponent* ASC = NativeGetWarriorAscFromActor(InActor);
	return ASC->HasMatchingGameplayTag(InTag);	
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, const FGameplayTag InTag,
	EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHasTag(InActor, InTag) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);
	if (const IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActorInfo(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* PawnCombatComponent = NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType = PawnCombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;
	return PawnCombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn);
	check(TargetPawn);
	
	IGenericTeamAgentInterface* QueryTeamAgentInterface = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	if (QueryTeamAgentInterface == nullptr)
		return false;

	IGenericTeamAgentInterface* TargetTeamAgentInterface = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());
	if (TargetTeamAgentInterface == nullptr)
		return false;

	return QueryTeamAgentInterface->GetGenericTeamId() != TargetTeamAgentInterface->GetGenericTeamId();
	
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker);
	check(InVictim);

	
	FVector VictimForward = FVector() ;
	FVector VictorToAttackerNormalized = FVector();
	
	OutAngleDifference = NativeGetAngleDifference(InVictim, InAttacker, VictimForward, VictorToAttackerNormalized);

	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictorToAttackerNormalized);

	//have to multiply with -1 because UE uses the lefthand rule of cross product
	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}

	if (OutAngleDifference >= -46.f && OutAngleDifference <= 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference  < -45.f && OutAngleDifference >= -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.f || OutAngleDifference > 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}
	return WarriorGameplayTags::Shared_Status_HitReact_Front;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker);
	check(InDefender);
	
	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());
	return DotResult < -0.1f;
	
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorAscFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorAscFromActor(InTargetActor);
	
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::Countdown(UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, EWarriorCountdownActionInput CountdownInput,
	EWarriorCountdownActionOutput& CountdownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;
	if (GEngine)
	{
		World =	GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}
	if (World ==nullptr)
	{
		return;
	}
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
	if (CountdownInput == EWarriorCountdownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FWarriorCountDownAction(TotalTime, UpdateInterval,
				OutRemainingTime, CountdownOutput, LatentInfo));
		}
	}
	if (CountdownInput == EWarriorCountdownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancleAction();
		}
	}
}


