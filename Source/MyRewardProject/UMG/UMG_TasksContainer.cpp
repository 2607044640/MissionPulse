// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_TasksContainer.h"

#include "UMG_BasicTask.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"


void UUMG_TasksContainer::TaskFinish(UUMG_BasicTask* Uumg_BasicTask)
{
	if (!ScrollBox_Tasks_Finish->HasChild(Uumg_BasicTask))
	{
		ScrollBox_Tasks_Finish->AddChild(Uumg_BasicTask);
	}
}

void UUMG_TasksContainer::TaskNotFinish(UUMG_BasicTask* Uumg_BasicTask)
{
	if (!ScrollBox_Tasks->HasChild(Uumg_BasicTask))
	{
		ScrollBox_Tasks->AddChild(Uumg_BasicTask);
	}
}

void UUMG_TasksContainer::ButtonAddTaskOnClick()
{
	UUMG_BasicTask* BasicTask = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), UIClass);
}

void UUMG_TasksContainer::NativeConstruct()
{
	Super::NativeConstruct();

	UMySaveGIS* MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
	for (FTaskData
	     InTaskData : MySaveGIS->Global_AllDataToSave.TaskDatum)
	{
		UUMG_BasicTask* BasicTask = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), UIClass);
		BasicTask->TaskData = InTaskData;
		if (BasicTask->TaskData.SavedTimes)
		{
			ScrollBox_Tasks->AddChild(BasicTask);
		}
		else
		{
			ScrollBox_Tasks_Finish->AddChild(BasicTask);
		}
	}

	ButtonAddTask->OnReleased.AddDynamic(this, &UUMG_TasksContainer::ButtonAddTaskOnClick);
}
