// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyRewardProject/GameInstanceSubsystems/MyRewardGIS.h"

// 	FString SortName;
//
// 	FString Title;
// 	FString Detail;
//
// 	float Score;
//
// 	int32 Days;
// 	int32 Frequency; //how many times per days?

void UUMG_BasicTask::TaskFinish(FTaskData& InTaskData, UUMG_BasicTask* BasicTask)
{
	Button_Finish->SetIsEnabled(false);
	BPTaskFinishEffect();
}

void UUMG_BasicTask::Button_EditTaskOnClick()
{
}

void UUMG_BasicTask::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Finish->OnClicked.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnClicked);
	Button_EditTask->OnClicked.AddDynamic(this, &UUMG_BasicTask::Button_EditTaskOnClick);

	OnAddScore.AddUObject(this, &UUMG_BasicTask::AddScore);
	OnTaskFinish.AddUObject(this, &UUMG_BasicTask::TaskFinish);
	RefreshUI();
}

void UUMG_BasicTask::Button_FinishOnClicked()
{
	if (OnAddScore.IsBound())
	{
		OnAddScore.Broadcast(TaskData, this);
	}
	RefreshUI();
}

void UUMG_BasicTask::AddScore(FTaskData& InTaskData, UUMG_BasicTask* BasicTask)
{
	UMyRewardGIS* MyRewardGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMyRewardGIS>();
	MyRewardGIS->AddScore(InTaskData.Score);
	InTaskData.SavedTimes = InTaskData.SavedTimes - 1;
	if (!InTaskData.SavedTimes)
	{
		if (OnTaskFinish.IsBound())
		{
			OnTaskFinish.Broadcast(InTaskData, this);
		}
	}
	BPOnAddScoreEffect();
}

void UUMG_BasicTask::RefreshUI()
{
	SlotTitle->SetText(FText::FromString(TaskData.Title));
	SlotTimes->SetText(FText::AsNumber(TaskData.SavedTimes));
	SlotDays->SetText(FText::AsNumber(TaskData.Days));
	SlotScore->SetText(FText::AsNumber(TaskData.Score));
}
