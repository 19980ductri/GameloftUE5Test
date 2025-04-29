// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "WarriorEnumTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorFunctionLibrary.generated.h"

struct FGameplayEffectSpecHandle;
class UPawnCombatComponent;



class UWarriorAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static UWarriorAbilitySystemComponent* NativeGetWarriorAscFromActor(AActor* InActor);
	static float NativeGetAngleDifference(AActor* A, AActor* B, FVector& OutAForwardVector, FVector& OutAtoBNormalized);
	static bool NativeDoesActorHasTag(AActor* InActor, FGameplayTag InTag);
	
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLib")
	static void AddGameplayTagToActorIfNone(AActor* InActor,FGameplayTag InTag);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLib")
	static void RemoveGameplayTagIfFound(AActor* InActor, FGameplayTag InTag);
	

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLib", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag InTag, EWarriorConfirmType& OutConfirmType);
	
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLib", meta = (DisplayName = "Get Pawn Combat Component From Actor Info", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActorInfo(AActor* InActor, EWarriorValidType& OutValidType);
	
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLib")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
	
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLib", meta = (CompactNodeTitle = "Get Value at Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel);
	
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLib")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);
    
	UFUNCTION(Blueprintpure, Category = "Warrior|FunctionLib")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);
	
	UFUNCTION(BlueprintCallable, Blueprintpure, Category = "Warrior|FunctionLib")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLib", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo", ExpandEnumAsExecs = "CountdownInput|CountdownOutput", TotalTime = "1.0", UpdateInterval = "0.1"))
	static void Countdown(UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime,
		EWarriorCountdownActionInput CountdownInput,
		UPARAM(DisplayName = "") EWarriorCountdownActionOutput& CountdownOutput,
		FLatentActionInfo LatentInfo);
	
};