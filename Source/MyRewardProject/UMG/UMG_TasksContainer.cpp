// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_TasksContainer.h"

#include "BFL_FunctionUtilities.h"
#include "UMG_BasicEditer.h"
#include "UMG_BasicTask.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "MyRewardProject/MyRewardProject.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"


void UUMG_TasksContainer::TaskFinish(UUMG_BasicTask* Uumg_BasicTask)
{
	if (!ScrollBox_Tasks_Finish->HasChild(Uumg_BasicTask))
	{
		ScrollBox_Tasks_Finish->AddChild(Uumg_BasicTask);
	}
}

void UUMG_TasksContainer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (bCanScroll)
	{
		SelectedScrollBox->SetScrollOffset(SelectedScrollBox->GetScrollOffset()
			+ (bIsScrollUp
				   ? -SpeedOfScroll
				   : SpeedOfScroll) * InDeltaTime * SpeedOfScroll_ByEdgeDistance);
	}
	Super::NativeTick(MyGeometry, InDeltaTime);
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
	BasicTask->TaskData.bIsAddScore = false;

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
	if (int32 TempAnotherDay = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>()->AnotherDay)
	{
		MySaveGIS->Global_AllDataToSave.GlobalDailyProgress_Saved = 0;

		BasicTask->TaskData.SavedDays -= TempAnotherDay;
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

void UUMG_TasksContainer::BasicEditer_GlobalDailyProgressOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText Text)
{
	MySaveGIS->Global_AllDataToSave.GlobalDailyProgress = FCString::Atoi(*Text.ToString());
}

void UUMG_TasksContainer::BasicEditer_DailyProgressRewardValueOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText Text)
{
	MySaveGIS->Global_AllDataToSave.DailyProgressRewardValue = FCString::Atoi(*Text.ToString());
}

void UUMG_TasksContainer::NativeConstruct()
{
	Super::NativeConstruct();
	//Init
	TArray<FString> Temp_SortNames;
	Temp_SortNames.Add(InitalName_AllTasks);
	ComboBoxString_TasksClassification->AddOption(InitalName_AllTasks);
	ComboBoxString_TasksClassification->SetSelectedOption(InitalName_AllTasks);

	BasicEditer_GlobalDailyProgress->OnEditFinish.AddUObject(
		this, &UUMG_TasksContainer::BasicEditer_GlobalDailyProgressOnEditFinish);
	BasicEditer_DailyProgressRewardValue->OnEditFinish.AddUObject(
		this, &UUMG_TasksContainer::BasicEditer_DailyProgressRewardValueOnEditFinish);

	//Bind Functions
	ComboBoxString_TasksClassification->OnSelectionChanged.AddDynamic(
		this, &UUMG_TasksContainer::ComboBoxString_TasksClassification_OnSelectionChanged);
	ButtonAddTask->OnReleased.AddDynamic(this, &UUMG_TasksContainer::ButtonAddTaskOnClick);

	//compare and add SortName
	MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
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


	//Init UI
	TextBlock_Score->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.GlobalTotalScore));
	TextBlock_GlobalDailyProgress_Saved->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.GlobalDailyProgress_Saved));
	BasicEditer_GlobalDailyProgress->TextBlock->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.GlobalDailyProgress));
	BasicEditer_DailyProgressRewardValue->TextBlock->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.DailyProgressRewardValue));

	MySaveGIS->SaveAllData();
}

FString UUMG_TasksContainer::FloatToText(float Input)
{
	return FString::SanitizeFloat(Input);
}

float UUMG_TasksContainer::TextBlockTextTofloat(UTextBlock* TextBlock)
{
	return FCString::Atof(*TextBlock->GetText().ToString());
}


bool UUMG_TasksContainer::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                           UDragDropOperation* InOperation)
{
	if (UUMG_BasicTask* Uumg_BasicTask = Cast<UUMG_BasicTask>(InOperation->Payload))
	{
		/// Selection of ScrollBox
		bool bTempCheck = UBFL_FunctionUtilities::CheckMouseIsInsideOfWidget_DragDropEvent(
			this, ScrollBox_Tasks, InDragDropEvent.GetScreenSpacePosition());
		SelectedScrollBox = bTempCheck ? ScrollBox_Tasks : ScrollBox_Tasks_Finish;

		//if two scroll box is visible, then set one of them to collapsed
		if (ScrollBox_Tasks->GetVisibility() == ESlateVisibility::Visible &&
			ScrollBox_Tasks_Finish->GetVisibility() == ESlateVisibility::Visible)
		{
			(bTempCheck ? ScrollBox_Tasks_Finish : ScrollBox_Tasks)->SetVisibility(ESlateVisibility::Collapsed);
		}

		//Use MousePosition - Viewport to get Local MousePosition value of inside of scrollbox  
		FVector2D LocalMousePositionInWidget = InDragDropEvent.GetScreenSpacePosition() -
			UBFL_FunctionUtilities::JFGetWidgetViewPortPosition(this, SelectedScrollBox);

		FVector2D RightDownCorner = SelectedScrollBox->GetCachedGeometry().GetLocalSize();


		if (LocalMousePositionInWidget.Y > RightDownCorner.Y - OffsetOfScroll)
		{
			// SpeedOfScroll_ByEdgeDistance = 5.f / RightDownCorner.Y - LocalMousePositionInWidget.Y;
			bCanScroll = true;
			bIsScrollUp = false;
		}
		else if (LocalMousePositionInWidget.Y < OffsetOfScroll)
		{
			// SpeedOfScroll_ByEdgeDistance = 5.f / LocalMousePositionInWidget.Y;
			bCanScroll = true;
			bIsScrollUp = true;
		}
		else if (LocalMousePositionInWidget.Y > OffsetOfScroll &&
			LocalMousePositionInWidget.Y < RightDownCorner.Y - OffsetOfScroll)
		{
			bCanScroll = false;
		}

		//get children position then calculate
		int32 InMyTempIndex = CalcAndGetIndex(InDragDropEvent.GetScreenSpacePosition(), SelectedScrollBox);
		//when the index of child is exactly the number has been calculated, then we don't need to operate anything
		if (InMyTempIndex != SelectedScrollBox->GetChildIndex(Uumg_BasicTask))
		{
			//Other operation
			MyInsertChildAt(InMyTempIndex, Uumg_BasicTask, SelectedScrollBox);

			SortPanelWidgetsChildren(SelectedScrollBox);

			MySaveGIS->SaveAllData();
		}
	}

	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

UPanelSlot* UUMG_TasksContainer::MyInsertChildAt(int32 Index, UWidget* Content, UScrollBox* ScrollBox)
{
	UPanelSlot* NewSlot = ScrollBox->AddChild(Content);
	int32 CurrentIndex = ScrollBox->GetChildIndex(Content);
	ScrollBox->Slots.RemoveAt(CurrentIndex);
	ScrollBox->Slots.Insert(Content->Slot, FMath::Clamp(Index, 0, ScrollBox->Slots.Num()));
	return NewSlot;
}


int32 UUMG_TasksContainer::CalcAndGetIndex(FVector2D MousePosition, UPanelWidget* InPanelWidget)
{
	UWidget* SelectChild = nullptr;

	for (UWidget* Child : InPanelWidget->GetAllChildren())
	{
		FGeometry ChildGeometry = Child->GetCachedGeometry();

		// Convert Child's Local Position to Screen/Absolute Position
		FVector2D ChildAbsolutePosition = ChildGeometry.GetAbsolutePosition();
		FVector2D ChildLocalSize = ChildGeometry.GetLocalSize();

		// Adjust Mouse Y Position relative to the middle of the child widget
		float AdjustedMouseY = MousePosition.Y - (ChildLocalSize.Y / 2.0f);

		// Check if the mouse is within the bounds of this child widget
		if (AdjustedMouseY > ChildAbsolutePosition.Y)
		{
			SelectChild = Child;
		}
	}

	// Return the index of the selected child widget or 0 if none found
	if (SelectChild)
	{
		return InPanelWidget->GetChildIndex(SelectChild) + 1;
	}
	return 0;
}

void UUMG_TasksContainer::SortPanelWidgetsChildren(UPanelWidget* InPanelWidget)
{
	TArray<UWidget*> Children = InPanelWidget->GetAllChildren();

	Children.Sort([InPanelWidget](const UWidget& A, const UWidget& B)
	{
		int32 IndexA = InPanelWidget->GetChildIndex(&A);
		int32 IndexB = InPanelWidget->GetChildIndex(&B);
		return IndexA < IndexB;
	});

	InPanelWidget->ClearChildren();
	for (UWidget* Child : Children)
	{
		InPanelWidget->AddChild(Child);
	}
}
