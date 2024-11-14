// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncNode_IntervalCall.h"

UAsyncNode_IntervalCall* UAsyncNode_IntervalCall::CreateAsyncIntervalCall(UObject* WorldContextObject, float IntervalCall,
                                                                        float MaxDuration)
{
	UAsyncNode_IntervalCall* AsyncNode_IntervalCall = NewObject<UAsyncNode_IntervalCall>();
	AsyncNode_IntervalCall->WorldContextObject = WorldContextObject;
	AsyncNode_IntervalCall->IntervalCall = IntervalCall;
	AsyncNode_IntervalCall->MaxDuration = MaxDuration;

	return AsyncNode_IntervalCall;
}

void UAsyncNode_IntervalCall::ResetVariables()
{
	CurrentTotalTime = 0;
	World->GetTimerManager().ClearTimer(TimeHandle);
	SetReadyToDestroy();
}

void UAsyncNode_IntervalCall::CancelUpdate()
{
	ResetVariables();
	OnCancel.Broadcast();
}

void UAsyncNode_IntervalCall::UpdateFunction()
{
	CurrentTotalTime += IntervalCall;
	if (MaxDuration > CurrentTotalTime || MaxDuration == -1)
	{
		OnUpdate.Broadcast();
	}
	else
	{
		ResetVariables();
		OnCompleted.Broadcast();
	}
}

void UAsyncNode_IntervalCall::Activate()
{
	Super::Activate();
	World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	World->GetTimerManager().SetTimer(TimeHandle, this, &UAsyncNode_IntervalCall::UpdateFunction, IntervalCall, true);
}
