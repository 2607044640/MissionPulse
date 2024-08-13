// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_Shop.h"

#include "UMG_BasicTask.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"


void UUMG_Shop::TaskFinish(FTaskData& TaskData, UUMG_BasicTask* Uumg_BasicTask)
{
	ScrollBox_Tasks_Finish->AddChild(Uumg_BasicTask);
	ScrollBox_Tasks->RemoveChild(Uumg_BasicTask);
}

void UUMG_Shop::ButtonAddTaskOnClick()
{
	
}

void UUMG_Shop::NativeConstruct()
{
	Super::NativeConstruct();

	
	UMySaveGIS* MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
	for (FTaskData
	     InTaskData : MySaveGIS->Global_AllDataToSave.TaskDatum)
	{
		UUMG_BasicTask* BasicTask = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), UIClass);
		BasicTask->TaskData = InTaskData;
		ScrollBox_Tasks->AddChild(BasicTask);
		BasicTask->AddToViewport();

		BasicTask->OnTaskFinish.AddUObject(this, &UUMG_Shop::TaskFinish);
	}

	ButtonAddTask->OnClicked.AddDynamic(this, &UUMG_Shop::ButtonAddTaskOnClick);
}
