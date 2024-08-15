// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "UMG_TasksContainer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MyRewardProject/GameInstanceSubsystems/MyRewardGIS.h"


void UUMG_BasicTask::TaskFinish(FTaskData& InTaskData, UUMG_BasicTask* BasicTask)
{
	Button_Finish->SetIsEnabled(false);
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

	UUMG_TasksContainer* OwningWidget = GetParent()->GetTypedOuter<UUMG_TasksContainer>();
	OnTaskFinish.AddUObject(OwningWidget, &UUMG_TasksContainer::TaskFinish);

	if (!TaskData.SavedTimes)
	{
		if (OnTaskFinish.IsBound())
		{
			OnTaskFinish.Broadcast(TaskData,this);
		}
	}
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
	FString TempStr = FString::Printf(TEXT("%i"),InTaskData.SavedTimes);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
	UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

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
	SlotSavedTimes->SetText(FText::AsNumber(TaskData.SavedTimes));
	SlotTimes->SetText(FText::AsNumber(TaskData.Times));
	SlotDays->SetText(FText::AsNumber(TaskData.Days));
	SlotScore->SetText(FText::AsNumber(TaskData.Score));
}
