// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorCountDownAction.h"


void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel == true)
	{
		CountdownOutput = EWarriorCountdownActionOutput::Canceled;
		Response.FinishAndTriggerIf(true, ExecutionFunction,OutputLink, CallbackTarget);
		return;
	}

	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		CountdownOutput = EWarriorCountdownActionOutput::Completed;
		Response.FinishAndTriggerIf(true, ExecutionFunction,OutputLink, CallbackTarget);
		return;
	}

	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();
		OutRemainTime = TotalCountDownTime - ElapsedTimeSinceStart;
		CountdownOutput = EWarriorCountdownActionOutput::Update;
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
		ElapsedInterval = 0;
	}
}	

void FWarriorCountDownAction::CancleAction()
{
	bNeedToCancel = true;
	
}
