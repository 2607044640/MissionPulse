// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "UMG_BasicEditer.h"
#include "UMG_TasksContainer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyRewardProject/GameInstanceSubsystems/MyRewardGIS.h"


void UUMG_BasicTask::TaskFinish(UUMG_BasicTask* BasicTask)
{
	Button_Finish->SetIsEnabled(false);
}

void UUMG_BasicTask::SlotScoreOnEditFinish(UUMG_BasicTask* InUumg_BasicTask,FText InText)
{
	TaskData.Score= FCString::Atoi(*InText.ToString());  
}

void UUMG_BasicTask::SlotTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask,FText InText)
{
	TaskData.Times= FCString::Atoi(*InText.ToString());  

}

void UUMG_BasicTask::SlotSavedTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask,FText InText)
{
	TaskData.SavedTimes= FCString::Atoi(*InText.ToString());  

}

void UUMG_BasicTask::SlotTitleOnEditFinish(UUMG_BasicTask* Uumg_BasicTask,FText InText)
{
	TaskData.Title= InText.ToString();  

}

void UUMG_BasicTask::NativeConstruct()
{
	Super::NativeConstruct();
	Button_Finish->OnClicked.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnClicked);

	OnAddScore.AddUObject(this, &UUMG_BasicTask::AddScore);
	OnTaskFinish.AddUObject(this, &UUMG_BasicTask::TaskFinish);

	UUMG_TasksContainer* OwningWidget = GetParent()->GetTypedOuter<UUMG_TasksContainer>();
	OnTaskFinish.AddUObject(OwningWidget, &UUMG_TasksContainer::TaskFinish);

	if (!TaskData.SavedTimes)
	{
		if (OnTaskFinish.IsBound())
		{
			OnTaskFinish.Broadcast(this);
		}
	}

	RefreshUI();
	
	SlotTitle->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTitleOnEditFinish);
	SlotSavedTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedTimesOnEditFinish);
	SlotTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTimesOnEditFinish);
	SlotScore->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotScoreOnEditFinish);
}

void UUMG_BasicTask::Button_FinishOnClicked()
{
	if (OnAddScore.IsBound())
	{
		OnAddScore.Broadcast(this);
	}
	RefreshUI();
}

void UUMG_BasicTask::AddScore(UUMG_BasicTask* BasicTask)
{
	UMyRewardGIS* MyRewardGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMyRewardGIS>();
	MyRewardGIS->AddScore(BasicTask->TaskData.Score);
	BasicTask->TaskData.SavedTimes = BasicTask->TaskData.SavedTimes - 1;

	if (!BasicTask->TaskData.SavedTimes)
	{
		if (OnTaskFinish.IsBound())
		{
			OnTaskFinish.Broadcast(this);
		}
	}
	BPOnAddScoreEffect();
	
}

void UUMG_BasicTask::RefreshUI()
{
	SlotTitle->TextBlock->SetText(FText::FromString(TaskData.Title));
	SlotSavedTimes->TextBlock->SetText(FText::AsNumber(TaskData.SavedTimes));
	SlotTimes->TextBlock->SetText(FText::AsNumber(TaskData.Times));
	SlotDays->TextBlock->SetText(FText::AsNumber(TaskData.Days));
	SlotScore->TextBlock->SetText(FText::AsNumber(TaskData.Score));
}
