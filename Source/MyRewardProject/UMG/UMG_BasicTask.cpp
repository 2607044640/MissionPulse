// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "BFL_FunctionUtilities.h"
#include "UMG_BasicEditer.h"
#include "UMG_MainUI.h"
#include "UMG_TasksContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
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

	if (bIsCopiedWidget) return;
	//button
	if (OnAddScore.IsBound())
	{
		return;
	}
	
	OnBasicTaskDrop.AddDynamic(this, &UUMG_BasicTask::BPOnDrop);

	Image_Coin->OnMouseButtonDownEvent.BindUFunction(this,TEXT("OnImageClicked"));

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
	SlotTitle->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTitleOnEditFinish);
	SlotSavedTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedTimesOnEditFinish);
	SlotTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTimesOnEditFinish);
	SlotScore->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotScoreOnEditFinish);
	SlotSavedDays->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedDaysOnEditFinish);
	SlotDays->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotDaysOnEditFinish);

	//Other
	MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &UUMG_BasicTask::CheckIfTaskFinish, 0.2);

	SetPadding(FMargin(5, 0, 200, 0));

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

FReply UUMG_BasicTask::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

bool UUMG_BasicTask::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	if (OnBasicTaskDrop.IsBound())
	{
		OnBasicTaskDrop.Broadcast();
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
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

void UUMG_BasicTask::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                          UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	OutOperation->Payload = this;
	OutOperation->DefaultDragVisual = this;
	OutOperation->Pivot = EDragPivot::MouseDown;

	BPOnDrag();
	RemoveFromParent();
}



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
