// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "BFL_FunctionUtilities.h"
#include "UMG_BasicEditer.h"
#include "UMG_MainUI.h"
#include "UMG_TasksContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyRewardProject/Frameworks/MyHUD.h"


class IAssetRegistry;
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
	if (TaskData.SavedTimes && bBasicTaskIsAddTask)
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

	if (TaskData.bIsAddScore)
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
	bBasicTaskIsAddTask = true;
	TaskData.SavedTimes = TaskData.SavedTimes + 1;

	if (TaskData.bIsAddScore)
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
	bBasicTaskIsAddTask = false;
}

void UUMG_BasicTask::Button_FinishOnPressed()
{
	GetWorld()->GetTimerManager().SetTimer(CheckPressedAddOrMinusHandle, this, &UUMG_BasicTask::CheckPressedAddOrMinus,
	                                       CheckPressedAddOrMinusRate);
}

void UUMG_BasicTask::ButtonAddScoreOnClicked()
{
	int32 TempINT32 = FCString::Atoi(*SlotScore->TextBlock->GetText().ToString());
	TempINT32 += 10;
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
	TempINT32 -= 10;
	if (TempINT32 < 0)
	{
		TempINT32 = 0;
	}
	TaskData.Score = TempINT32;
	RefreshUI();
	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::ButtonSelectOnClick()
{
	if (umg_ParentTasksContainer->SelectedBasicTask == this)
	{
		umg_ParentTasksContainer->RemoveAllSelectedBasicTask();
		return;
	}

	umg_ParentTasksContainer->RemoveAllSelectedBasicTask();
	umg_ParentTasksContainer->SelectedBasicTask = this;
	if (OnBasicTaskSelected.IsBound())
	{
		OnBasicTaskSelected.Broadcast();
	}
}

void UUMG_BasicTask::NativeConstruct()
{
	Super::NativeConstruct();

	if (bIsCopiedWidget) return;
	//button
	if (OnAddScore.IsBound())
	{
		return;
	}

	PreviousColor = Border_UserVisualColor->GetBrushColor();
	OnBasicTaskUnselected.AddDynamic(this, &UUMG_BasicTask::TaskOnUnSelected);
	OnBasicTaskSelected.AddDynamic(this, &UUMG_BasicTask::TaskOnSelected);

	Image_Coin->OnMouseButtonDownEvent.BindUFunction(this,TEXT("OnImageClicked"));

	ButtonSelect->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonSelectOnClick);
	Button_Finish->OnClicked.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnClicked);
	Button_Finish->OnPressed.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnPressed);
	OnAddScore.AddUObject(this, &UUMG_BasicTask::AddScore);
	OnMinusScore.AddUObject(this, &UUMG_BasicTask::MinusScore);
	OnButtonClicked.AddUObject(this, &UUMG_BasicTask::ButtonClicked);
	ButtonAddScore->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonAddScoreOnClicked);

	ButtonMinusScore->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonMinusScoreOnClicked);

	//finish
	OnTaskFinish.AddUObject(this, &UUMG_BasicTask::TaskFinish);
	AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	umg_ParentTasksContainer = MyHUD->MainUI->TasksContainer;
	if (!umg_ParentTasksContainer)
	{
		RemoveFromParent();
		FString teowjewo = FString::Printf(TEXT("None Parent_TasksContainer->RemoveFromParent"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, teowjewo, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *teowjewo);
	}

	OnTaskFinish.AddUObject(umg_ParentTasksContainer, &UUMG_TasksContainer::TaskFinish);
	OnTaskNotFinish.AddUObject(umg_ParentTasksContainer, &UUMG_TasksContainer::TaskNotFinish);

	//edit
	SlotTitle->OnEditFinishedCommitted.AddUObject(this, &UUMG_BasicTask::SlotTitleOnEditFinish);
	SlotSavedTimes->OnEditFinishedCommitted.AddUObject(this, &UUMG_BasicTask::SlotSavedTimesOnEditFinish);
	SlotTimes->OnEditFinishedCommitted.AddUObject(this, &UUMG_BasicTask::SlotTimesOnEditFinish);
	SlotScore->OnEditFinishedCommitted.AddUObject(this, &UUMG_BasicTask::SlotScoreOnEditFinish);
	SlotSavedDays->OnEditFinishedCommitted.AddUObject(this, &UUMG_BasicTask::SlotSavedDaysOnEditFinish);
	SlotDays->OnEditFinishedCommitted.AddUObject(this, &UUMG_BasicTask::SlotDaysOnEditFinish);

	//Other
	MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &UUMG_BasicTask::CheckIfTaskFinish, 0.2);

	SetPadding(BasicTaskMargin);

	RefreshUI();
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


bool UUMG_BasicTask::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	if (OnBasicTaskUnselected.IsBound())
	{
		OnBasicTaskUnselected.Broadcast();
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UUMG_BasicTask::TaskOnSelected()
{
	Border_UserVisualColor->SetBrushColor(BP_Border_UserVisualColor_Color);
}

void UUMG_BasicTask::TaskOnUnSelected()
{
	if (this == umg_ParentTasksContainer->SelectedBasicTask)
	{
		Border_UserVisualColor->SetBrushColor(PreviousColor);
		BPTaskOnUnSelectedHideTimesAndDaysUMG();
	}
}

// UUMG_BasicTask* UUMG_BasicTask::CopySelf()
// {
// 	if (auto ThisWidget = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), ParentTasksContainer->UIClass))
// 	{
// 		ThisWidget->bIsCopiedWidget = true;
// 		ThisWidget->TaskData = TaskData;
// 		ThisWidget->RefreshUI();
//
// 		return ThisWidget;
// 	}
//
// 	FString ewg = FString::Printf(TEXT("CopySelf Not Valid"));
// 	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, ewg, false);
// 	UE_LOG(LogTemp, Error, TEXT("%s"), *ewg);
//
// 	return nullptr;
// }


void UUMG_BasicTask::RefreshUI()
{
	SlotTitle->TextBlock->SetText(FText::FromString(TaskData.Title));
	SlotSavedTimes->TextBlock->SetText(UBFL_FunctionUtilities::JFFloatToText(TaskData.SavedTimes));
	SlotTimes->TextBlock->SetText(UBFL_FunctionUtilities::JFFloatToText(TaskData.Times));
	SlotDays->TextBlock->SetText(UBFL_FunctionUtilities::JFFloatToText(TaskData.Days));
	SlotSavedDays->TextBlock->SetText(UBFL_FunctionUtilities::JFFloatToText(TaskData.SavedDays));
	SlotScore->TextBlock->SetText(UBFL_FunctionUtilities::JFFloatToText(TaskData.Score));

	RefreshImage();
	BPOtherRefresh();
}


void UUMG_BasicTask::RefreshImage()
{
	if (TaskData.bIsAddScore)
	{
		Image_Coin->SetColorAndOpacity(GoldColor);
	}
	else
	{
		Image_Coin->SetColorAndOpacity(RedGoldColor);
	}
}

FEventReply UUMG_BasicTask::OnImageClicked(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	TaskData.bIsAddScore = !TaskData.bIsAddScore;

	RefreshImage();

	MySaveGIS->SaveAllData();

	FEventReply TempEvent;
	return TempEvent;
}
