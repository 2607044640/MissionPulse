// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActionNode.h"

#include "Kismet/KismetSystemLibrary.h"


UAsyncActionNode* UAsyncActionNode::MultiThreadedNode()
{
	UAsyncActionNode* Node = NewObject<UAsyncActionNode>();
	return Node;
}

void UAsyncActionNode::Activate()
{
	(new FAutoDeleteAsyncTask<BP_NonAbandonableTask>(this))->StartBackgroundTask();
}

//====================================================================================

BP_NonAbandonableTask::BP_NonAbandonableTask(UAsyncActionNode* BP_TaskInstance)
{
	CallingObject = BP_TaskInstance;
}

BP_NonAbandonableTask::~BP_NonAbandonableTask()
{
	CallingObject->FinishedWork.Broadcast();
	CallingObject->SetReadyToDestroy();
}

TStatId BP_NonAbandonableTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(BP_NonAbandonableTask, STATGROUP_ThreadPoolAsyncTasks);
}

void BP_NonAbandonableTask::DoWork()
{
	CallingObject->MultiThreadedWork.Broadcast();
}
