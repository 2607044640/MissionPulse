// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "UMG_BasicEditer.h"
#include "UMG_TasksContainer.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


class AMyHUD;

void UUMG_BasicTask::TaskFinish(UUMG_BasicTask* BasicTask)
{
	// Button_Finish->SetIsEnabled(false);
}

void UUMG_BasicTask::SlotScoreOnEditFinish(UUMG_BasicTask* InUumg_BasicTask, FText InText)
{
	TaskData.Score = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.Times = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotSavedTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.SavedTimes = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotTitleOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.Title = InText.ToString();
}

void UUMG_BasicTask::SlotSavedDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.SavedDays = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.Days = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::ButtonClicked(UUMG_BasicTask* Uumg_BasicTask)
{
	UKismetSystemLibrary::K2_PauseTimerHandle(this, CheckPressedAddOrMinusHandle);
	if (TaskData.SavedTimes && IsAddTask)
	{
		if (OnAddScore.IsBound())
		{
			OnAddScore.Broadcast(this);
		}
	}
	else
	{
		if (OnMinusScore.IsBound())
		{
			OnMinusScore.Broadcast(this);
		}
	}

	CheckIfTaskFinish();

	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::AddScore(UUMG_BasicTask* BasicTask)
{
	TaskData.SavedTimes = TaskData.SavedTimes - 1;

	if (TaskIsAddScore)
	{
		MySaveGIS->AddScore(TaskData.Score);
		BPOnAddScoreEffect();
	}
	else
	{
		MySaveGIS->MinusScore(TaskData.Score);
		BPOnMinusScoreEffect();
	}
}

void UUMG_BasicTask::MinusScore(UUMG_BasicTask* Uumg_BasicTask)
{
	IsAddTask = true;
	TaskData.SavedTimes = TaskData.SavedTimes + 1;
	if (TaskIsAddScore)
	{
		MySaveGIS->MinusScore(TaskData.Score);
		BPOnMinusScoreEffect();
	}
	else
	{
		MySaveGIS->AddScore(TaskData.Score);
		BPOnAddScoreEffect();
	}
}

void UUMG_BasicTask::CheckPressedAddOrMinus()
{
	IsAddTask = false;
}

void UUMG_BasicTask::Button_FinishOnPressed()
{
	GetWorld()->GetTimerManager().SetTimer(CheckPressedAddOrMinusHandle, this, &UUMG_BasicTask::CheckPressedAddOrMinus,
	                                       CheckPressedAddOrMinusRate);
}

void UUMG_BasicTask::ButtonAddScoreOnClicked()
{
	int32 TempINT32 = FCString::Atoi(*SlotScore->TextBlock->GetText().ToString());
	TempINT32++;
	if (TempINT32 < 0)
	{
		TempINT32 = 0;
	}
	TaskData.Score = TempINT32;
	RefreshUI();
	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::ButtonMinusScoreOnClicked()
{
	int32 TempINT32 = FCString::Atoi(*SlotScore->TextBlock->GetText().ToString());
	TempINT32--;
	if (TempINT32 < 0)
	{
		TempINT32 = 0;
	}
	TaskData.Score = TempINT32;
	RefreshUI();
	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::NativeConstruct()
{
	Super::NativeConstruct();

	//button
	Button_Finish->OnClicked.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnClicked);
	Button_Finish->OnPressed.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnPressed);
	OnAddScore.AddUObject(this, &UUMG_BasicTask::AddScore);
	OnMinusScore.AddUObject(this, &UUMG_BasicTask::MinusScore);
	OnButtonClicked.AddUObject(this, &UUMG_BasicTask::ButtonClicked);
	ButtonAddScore->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonAddScoreOnClicked);
	ButtonMinusScore->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonMinusScoreOnClicked);

	//finish
	OnTaskFinish.AddUObject(this, &UUMG_BasicTask::TaskFinish);
	TasksContainer = GetParent()->GetTypedOuter<UUMG_TasksContainer>();
	OnTaskFinish.AddUObject(TasksContainer, &UUMG_TasksContainer::TaskFinish);
	OnTaskNotFinish.AddUObject(TasksContainer, &UUMG_TasksContainer::TaskNotFinish);

	//edit
	SlotTitle->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTitleOnEditFinish);
	SlotSavedTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedTimesOnEditFinish);
	SlotTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTimesOnEditFinish);
	SlotScore->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotScoreOnEditFinish);
	SlotSavedDays->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedDaysOnEditFinish);
	SlotDays->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotDaysOnEditFinish);

	//Other
	MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();

	TaskIsAddScore = TaskData.bIsAddScore;

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &UUMG_BasicTask::CheckIfTaskFinish, 0.2);

	RefreshUI();

	FMargin Margin(5, 10, 200, 10);
	SetPadding(Margin);
}


void UUMG_BasicTask::Button_FinishOnClicked()
{
	if (OnButtonClicked.IsBound())
	{
		OnButtonClicked.Broadcast(this);
	}
	RefreshUI();
}

void UUMG_BasicTask::CheckIfTaskFinish()
{
	if (!TaskData.SavedTimes)
	{
		if (OnTaskFinish.IsBound())
		{
			OnTaskFinish.Broadcast(this);
		}
	}
	else
	{
		if (OnTaskNotFinish.IsBound())
		{
			OnTaskNotFinish.Broadcast(this);
		}
	}
}


void UUMG_BasicTask::RefreshUI()
{
	SlotTitle->TextBlock->SetText(FText::FromString(TaskData.Title));
	SlotSavedTimes->TextBlock->SetText(FText::AsNumber(TaskData.SavedTimes));
	SlotTimes->TextBlock->SetText(FText::AsNumber(TaskData.Times));
	SlotDays->TextBlock->SetText(FText::AsNumber(TaskData.Days));
	SlotSavedDays->TextBlock->SetText(FText::AsNumber(TaskData.SavedDays));
	SlotScore->TextBlock->SetText(FText::AsNumber(TaskData.Score));
	// AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	// MyHUD->MainUI->TasksContainer->SlotTotalScore->TextBlock->SetText(FText::AsNumber(MySaveGIS->GetScore()));

	BPOtherRefresh();
}

void UUMG_BasicTask::SetbIsAddScore()
{
	TaskData.bIsAddScore = TaskIsAddScore;
	MySaveGIS->SaveAllData();
}
