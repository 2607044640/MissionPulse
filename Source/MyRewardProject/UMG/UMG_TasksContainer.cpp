// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_TasksContainer.h"

#include "UMG_BasicTask.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "MyRewardProject/MyRewardProject.h"
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
	ScrollBox_Tasks->AddChild(BasicTask);
	BasicTask->TaskData.SortName = ComboBoxString_TasksClassification->GetSelectedOption();
	UMySaveGIS* MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
	MySaveGIS->SaveAllData();
}

void UUMG_TasksContainer::ComboBoxString_TasksClassification_OnSelectionChanged(FString SelectedItem,
	ESelectInfo::Type SelectionType)
{
	for (UWidget*
	     Child : ScrollBox_Tasks->GetAllChildren())
	{
		Child->SetVisibility(ESlateVisibility::Collapsed);
		if (UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(Child))
		{
			if (!SelectedItem.Compare(InitalName_AllTasks))
			{
				Child->SetVisibility(ESlateVisibility::Visible);
			}
			if (!UMG_BasicTask->TaskData.SortName.Compare(SelectedItem))
			{
				Child->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	for (UWidget*
	     Child : ScrollBox_Tasks_Finish->GetAllChildren())
	{
		Child->SetVisibility(ESlateVisibility::Collapsed);
		if (UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(Child))
		{
			if (!SelectedItem.Compare(InitalName_AllTasks))
			{
				Child->SetVisibility(ESlateVisibility::Visible);
			}
			if (!UMG_BasicTask->TaskData.SortName.Compare(SelectedItem))
			{
				Child->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UUMG_TasksContainer::TaskDataAddToTask(FTaskData InTaskData)
{
	//TaskData to BasicUmg
	UUMG_BasicTask* BasicTask = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), UIClass);
	BasicTask->TaskData = InTaskData;

	//Check AnotherDay
	int32 TempDay = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>()->AnotherDay;
	if (TempDay)
	{
		BasicTask->TaskData.SavedDays -= TempDay;
		if (BasicTask->TaskData.SavedDays < 0)
		{
			if (BasicTask->TaskData.Days)
			{
				BasicTask->TaskData.SavedTimes = BasicTask->TaskData.Times;
			}
			BasicTask->TaskData.SavedDays = BasicTask->TaskData.Days;
		}
	}

	if (BasicTask->TaskData.SavedTimes)
	{
		ScrollBox_Tasks->AddChild(BasicTask);
	}
	else
	{
		ScrollBox_Tasks_Finish->AddChild(BasicTask);
	}
}

void UUMG_TasksContainer::NativeConstruct()
{
	Super::NativeConstruct();
	//Init
	TArray<FString> Temp_SortNames;
	Temp_SortNames.Add(InitalName_AllTasks);
	ComboBoxString_TasksClassification->AddOption(InitalName_AllTasks);
	ComboBoxString_TasksClassification->SetSelectedOption(InitalName_AllTasks);

	//Bind Functions
	ComboBoxString_TasksClassification->OnSelectionChanged.AddDynamic(
		this, &UUMG_TasksContainer::ComboBoxString_TasksClassification_OnSelectionChanged);
	ButtonAddTask->OnReleased.AddDynamic(this, &UUMG_TasksContainer::ButtonAddTaskOnClick);

	//compare and add SortName
	UMySaveGIS* MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
	for (FTaskData
	     InTaskData : MySaveGIS->Global_AllDataToSave.TaskDatum)
	{
		bool TaskSortNameIsNew = true;
		for (FString
		     SortName : Temp_SortNames)
		{
			if (!InTaskData.SortName.Compare(SortName))
			{
				TaskSortNameIsNew = false;
			}
		}

		if (TaskSortNameIsNew)
		{
			Temp_SortNames.Add(InTaskData.SortName);
			ComboBoxString_TasksClassification->AddOption(InTaskData.SortName);
		}


		//Add tasks
		TaskDataAddToTask(InTaskData);
	}

	MySaveGIS->SaveAllData();
}

float UUMG_TasksContainer::NumberDeOrIncreaseGradually(float Number, float SavedNumber, float Speed, float LessThan)
{
	if (FMath::Abs(Number - SavedNumber) < LessThan)
	{
		BPDoTest();
		return FMath::RoundToInt32(Speed / 100.f * (SavedNumber > Number ? 1 : -1) + Number);
	}
	else
	{
		return Speed / 100.f * (SavedNumber > Number ? 1 : -1) + Number;
	}
}
