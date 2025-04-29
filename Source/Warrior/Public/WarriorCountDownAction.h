// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorEnumTypes.h"

class FWarriorCountDownAction : public FPendingLatentAction
{
public:
	FWarriorCountDownAction(float InTotalCountDownTime, float InUpdateInterval, float& InOutRemainTime, EWarriorCountdownActionOutput& InCountdownActionOutput,const FLatentActionInfo& InLatentInfo)
		:
		bNeedToCancel(false),
		TotalCountDownTime(InTotalCountDownTime),
		UpdateInterval(InUpdateInterval),
		OutRemainTime(InOutRemainTime),
		CountdownOutput(InCountdownActionOutput),
		ExecutionFunction(InLatentInfo.ExecutionFunction),
		OutputLink(InLatentInfo.Linkage),
		CallbackTarget(InLatentInfo.CallbackTarget),
		ElapsedInterval(0.f),
		ElapsedTimeSinceStart(0.f)
	{
	}


	virtual void UpdateOperation(FLatentResponse& Response) override;
	
	void CancleAction();
private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	float UpdateInterval;
	float& OutRemainTime;
	EWarriorCountdownActionOutput& CountdownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
	
};
