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
#include "Components/EditableTextBox.h"

void UUMG_TasksContainer::TaskFinish(UUMG_BasicTask* Uumg_BasicTask)
{
	if (!ScrollBox_Tasks_Finish->HasChild(Uumg_BasicTask))
	{
		ScrollBox_Tasks_Finish->AddChild(Uumg_BasicTask);
	}
}

void UUMG_TasksContainer::BroadcastBasicTaskUnselected(UUMG_BasicTask* UMG_BasicTask)
{
	if (UMG_BasicTask->OnBasicTaskUnselected.IsBound())
	{
		UMG_BasicTask->OnBasicTaskUnselected.Broadcast();
	}
}

void UUMG_TasksContainer::RemoveAllSelectedBasicTask()
{
	ExecuteFP_OperateChildren(this, &UUMG_TasksContainer::BroadcastBasicTaskUnselected);
	SelectedBasicTask = nullptr;
}

FReply UUMG_TasksContainer::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	RemoveAllSelectedBasicTask();
	if (TaskContainerOnMouseButtonDown.IsBound())
	{
		TaskContainerOnMouseButtonDown.Broadcast();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}


//todo 平移一格就是length/task count
void UUMG_TasksContainer::ScrollTheChildDown(bool IsDown, UWidget* InBasicTask)
{
	if (!InBasicTask)
	{
		return;
	}
	if (UScrollBox* SelectedScrollBox = Cast<UScrollBox>(InBasicTask->GetParent()))
	{
		UBFL_FunctionUtilities::SortPanelWidgetsChildren(SelectedScrollBox);
		//if two scroll box is visible, then set one of them to collapsed
		if (ScrollBox_Tasks->GetVisibility() == ESlateVisibility::Visible &&
			ScrollBox_Tasks_Finish->GetVisibility() == ESlateVisibility::Visible)
		{
			bool bTempCheck = (SelectedScrollBox == ScrollBox_Tasks);
			(bTempCheck ? ScrollBox_Tasks_Finish : ScrollBox_Tasks)->SetVisibility(ESlateVisibility::Collapsed);
		}

		//Other operation
		int32 TempIndex = SelectedScrollBox->GetChildIndex(InBasicTask);
		IsDown ? TempIndex-- : TempIndex++;

		//check if the task is collapsed. when it is,then index++ or --
		//This is to allow users to click only once and the task will be moved, otherwise task may won't move
		while (SelectedScrollBox->GetChildAt(TempIndex))
		{
			if (SelectedScrollBox->GetChildAt(TempIndex)->GetVisibility() == ESlateVisibility::Collapsed)
			{
				IsDown ? TempIndex-- : TempIndex++;
			}
			else
			{
				break;
			}
		}



		MyInsertChildAt(TempIndex, InBasicTask, SelectedScrollBox);

		UBFL_FunctionUtilities::SortPanelWidgetsChildren(SelectedScrollBox);

		MySaveGIS->SaveAllData();


		//set scroll offset
		float ChildrenCount = SelectedScrollBox->GetChildrenCount() - 1;

		float TotalLength = SelectedScrollBox->GetScrollOffsetOfEnd();

		float AverageLength = TotalLength / ChildrenCount;

		float FinalScrollOffset = AverageLength * SelectedScrollBox->GetChildIndex(InBasicTask);

		SelectedScrollBox->SetScrollOffset(FinalScrollOffset);
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

	//move new task to the top
	SelectedBasicTask = BasicTask;
	if (UScrollBox* SelectedScrollBox = Cast<UScrollBox>(SelectedBasicTask->GetParent()))
	{
		MyInsertChildAt(0, SelectedBasicTask, SelectedScrollBox);
		UBFL_FunctionUtilities::SortPanelWidgetsChildren(SelectedScrollBox);
		RemoveAllSelectedBasicTask();
	}

	//True init code
	BasicTask->TaskData.SortName = ComboBoxString_TasksClassification->GetSelectedOption();
	BasicTask->TaskData.bIsAddScore = true;
	BasicTask->TaskData.Score = 10;
	BasicTask->TaskData.SpawnTime = FDateTime::Now().GetTicks();
	BasicTask->TaskData.ClickTime = FDateTime::Now().GetTicks();

	BasicTask->RefreshUI();

	MySaveGIS->SaveAllData();

	ClearThenGenerateSortedOptions();
	ComboBoxString_TasksClassification->SetSelectedOption(BasicTask->TaskData.SortName);
}

void UUMG_TasksContainer::SetVisibilityWhenSelectionChanged(UUMG_BasicTask* UMG_BasicTask, FString SelectedItem)
{
	UMG_BasicTask->SetVisibility(ESlateVisibility::Collapsed);
	if (!SelectedItem.Compare(InitialName_AllTasks))
	{
		UMG_BasicTask->SetVisibility(ESlateVisibility::Visible);
	}
	if (!UMG_BasicTask->TaskData.SortName.Compare(SelectedItem))
	{
		UMG_BasicTask->SetVisibility(ESlateVisibility::Visible);
	}
}


template <typename Func>
void UUMG_TasksContainer::ExecuteForAllChildrenWithLambda(Func Function)
{
	for (UWidget* Child : ScrollBox_Tasks->GetAllChildren())
	{
		if (UUMG_BasicTask* CastedChild = Cast<UUMG_BasicTask>(Child))
		{
			Function(CastedChild);
		}
	}
	for (UWidget* Child : ScrollBox_Tasks_Finish->GetAllChildren())
	{
		if (UUMG_BasicTask* CastedChild = Cast<UUMG_BasicTask>(Child))
		{
			Function(CastedChild);
		}
	}
}

void UUMG_TasksContainer::ComboBoxString_TasksClassification_OnSelectionChanged(FString SelectedItem,
	ESelectInfo::Type SelectionType)
{
	if (bIsChangeSortName_Task && SelectedBasicTask)
	{
		if (TempSelectedBasicTask != SelectedBasicTask)
		{
			TempSelectedBasicTask = SelectedBasicTask;
			TempStringRecorderForSelection = SelectedBasicTask->TaskData.SortName;
		}
		//The order can not be wrong
		SelectedBasicTask->TaskData.SortName = SelectedItem;

		bIsChangeSortName_Task = false;
		ComboBoxString_TasksClassification->SetSelectedOption(TempStringRecorderForSelection);

		MySaveGIS->SaveAllData();
		return;
	}
	auto TaskFilter = [SelectedItem](UUMG_BasicTask* UMG_BasicTask)
	{
		UMG_BasicTask->SetVisibility(ESlateVisibility::Collapsed);
		if (!SelectedItem.Compare(InitialName_AllTasks))
		{
			UMG_BasicTask->SetVisibility(ESlateVisibility::Visible);
		}
		if (!UMG_BasicTask->TaskData.SortName.Compare(SelectedItem))
		{
			UMG_BasicTask->SetVisibility(ESlateVisibility::Visible);
		}
	};
	ExecuteForAllChildrenWithLambda(TaskFilter);

	// ExecuteForAllChildrenWithStdFunction<UUMG_BasicTask>(TaskFilter);
	// ExecuteFP_OperateChildren(this, &UUMG_TasksContainer::SetVisibilityWhenSelectionChanged, SelectedItem);
}


void UUMG_TasksContainer::TaskDataTransformToTask(FTaskData InTaskData)
{
	//Create widget
	UUMG_BasicTask* BasicTask = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), UIClass);
	BasicTask->TaskData = InTaskData;

	//Check AnotherDay
	if (int32 TempAnotherDay = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>()->AnotherDay)
	{
		//MySaveGIS->Global_AllDataToSave.GlobalDailyProgress_Saved = 0;

		BasicTask->TaskData.SavedDays -= TempAnotherDay;
		if (BasicTask->TaskData.SavedDays <= 0)
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
	BPOnDailyProgressEditFinish();
}

void UUMG_TasksContainer::BasicEditer_DailyProgressRewardValueOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText Text)
{
	MySaveGIS->Global_AllDataToSave.DailyProgressRewardValue = FCString::Atoi(*Text.ToString());
}

void UUMG_TasksContainer::ClearThenGenerateSortedOptions()
{
	// Get Selected Option to Switch back the option later
	FString TempOption;
	if (ComboBoxString_TasksClassification->GetSelectedOption().Compare(""))
	{
		TempOption = ComboBoxString_TasksClassification->GetSelectedOption();
	}


	//ComboBoxString_TasksClassification
	ComboBoxString_TasksClassification->ClearOptions();


	TArray<FString> Temp_SortNames;
	Temp_SortNames.Add(InitialName_AllTasks);
	ComboBoxString_TasksClassification->AddOption(InitialName_AllTasks);
	ComboBoxString_TasksClassification->SetSelectedOption(InitialName_AllTasks);

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
	}

	// Get Selected Option to Switch back the option we want
	if (TempOption.Compare(""))
	{
		ComboBoxString_TasksClassification->SetSelectedOption(TempOption);
	}
}

template <class T>
void CallAnyInAny(T* TAny, void (T::*AnyFunc)())
{
	(TAny->*AnyFunc)();
}

template <class TClass, class TMemberFunc, class... TArgs>
void UUMG_TasksContainer::ExecuteFP_OperateChildren(TClass* Instance, TMemberFunc Func, TArgs&&... Args)
{
	// Cache the children arrays to avoid multiple GetAllChildren() calls
	const TArray<UWidget*>& TasksChildren = ScrollBox_Tasks->GetAllChildren();
	const TArray<UWidget*>& TasksFinishChildren = ScrollBox_Tasks_Finish->GetAllChildren();

	// Pre-allocate expected size
	int32 TotalChildren = TasksChildren.Num() + TasksFinishChildren.Num();
	if (TotalChildren == 0) return;

	// Process main tasks
	for (UWidget* Child : TasksChildren)
	{
		if (UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(Child))
		{
			(Instance->*Func)(UMG_BasicTask, std::forward<TArgs>(Args)...);
		}
	}
	for (UWidget* Child : TasksFinishChildren)
	{
		if (UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(Child))
		{
			(Instance->*Func)(UMG_BasicTask, std::forward<TArgs>(Args)...);
		}
	}
}


void UUMG_TasksContainer::ChangeOption()
{
	//ChangeOption
	ExecuteFP_OperateChildren(this, &UUMG_TasksContainer::ChangeChildrenSortname,
	                          EditableTextBox_SortName->GetText());
	MySaveGIS->SaveAllData();
	ClearThenGenerateSortedOptions();
	ComboBoxString_TasksClassification->SetSelectedOption(EditableTextBox_SortName->GetText().ToString());
	EditableTextBox_SortName->SetVisibility(ESlateVisibility::Collapsed);
}

void UUMG_TasksContainer::EditableTextBox_SortNameOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	//fix press enter cause committed twice bug
	if (CommitMethod == ETextCommit::OnCleared)
	{
		return;
	}

	if (bIsAddOption)
	{
		BPAddOption();
		//implemented in blueprint
	}
	else
	{
		ChangeOption();
	}
	EditableTextBox_SortName->SetText(FText::FromString(""));
}

void UUMG_TasksContainer::Button_AddSortNameOnClicked()
{
	bIsAddOption = true;
	EditableTextBox_SortName->SetText(FText::FromString(""));
}

void UUMG_TasksContainer::Button_ChangeSortNamesOnClicked()
{
	// change visibility 
	if (EditableTextBox_SortName->GetVisibility() == ESlateVisibility::Collapsed
		&& ComboBoxString_TasksClassification->GetSelectedOption().Compare(InitialName_AllTasks))
	{
		EditableTextBox_SortName->SetVisibility(ESlateVisibility::Visible);
	}
	//if option is not equal all tasks then set text and set AddOption to false
	if (ComboBoxString_TasksClassification->GetSelectedOption().Compare(InitialName_AllTasks))
	{
		bIsAddOption = false;
		EditableTextBox_SortName->SetText(FText::FromString(ComboBoxString_TasksClassification->
			GetSelectedOption()));
	}
}

void UUMG_TasksContainer::ButtonChangeSortName_TaskOnClick1()
{
	if (!ComboBoxString_TasksClassification || !SelectedBasicTask)
	{
		return;
	}

	if (TSharedPtr<SComboBox<TSharedPtr<FString>>> ComboBox = StaticCastSharedPtr<SComboBox<TSharedPtr<FString>>>(
		ComboBoxString_TasksClassification->GetCachedWidget()))
	{
		ComboBox->SetIsOpen(true);
	}

	bIsChangeSortName_Task = true;
}

void UUMG_TasksContainer::RemoveChildrenThatVisible()
{
	ExecuteForAllChildrenWithLambda([&](UUMG_BasicTask* Child)
	{
		if (Child->IsVisible())
		{
			Child->RemoveFromParent();
		}
	});
}

void UUMG_TasksContainer::RegenerateTasksFromGlobalData()
{
	//Clear Stat
	ClearThenGenerateSortedOptions();

	ExecuteForAllChildrenWithLambda([&](UUMG_BasicTask* Child)
	{
		Child->RemoveFromParent();
	});

	//Add tasks
	for (FTaskData
	     InTaskData : MySaveGIS->Global_AllDataToSave.TaskDatum)
	{
		TaskDataTransformToTask(InTaskData);
	}
	//Init Global UI Value
	TextBlock_Score->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.GlobalTotalScore));
	TextBlock_GlobalDailyProgress_Saved->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.GlobalDailyProgress_Saved));
	BasicEditer_GlobalDailyProgress->TextBlock->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.GlobalDailyProgress));
	BasicEditer_DailyProgressRewardValue->TextBlock->SetText(
		UBFL_FunctionUtilities::JFFloatToText(MySaveGIS->Global_AllDataToSave.DailyProgressRewardValue));

	// MySaveGIS->SaveAllData();
}

void UUMG_TasksContainer::NativeConstruct()
{
	Super::NativeConstruct();

	//Binds
	ButtonChangeSortName_Task->OnClicked.AddDynamic(this, &ThisClass::ButtonChangeSortName_TaskOnClick1);
	Button_AddSortName->OnClicked.AddDynamic(this, &ThisClass::Button_AddSortNameOnClicked);
	Button_ChangeSortNames->OnPressed.AddDynamic(this, &ThisClass::Button_ChangeSortNamesOnClicked);
	EditableTextBox_SortName->OnTextCommitted.AddDynamic(
		this, &ThisClass::EditableTextBox_SortNameOnTextCommitted);
	BasicEditer_GlobalDailyProgress->OnEditFinishedCommitted.AddUObject(
		this, &ThisClass::BasicEditer_GlobalDailyProgressOnEditFinish);
	BasicEditer_DailyProgressRewardValue->OnEditFinishedCommitted.AddUObject(
		this, &ThisClass::BasicEditer_DailyProgressRewardValueOnEditFinish);
	ComboBoxString_TasksClassification->OnSelectionChanged.AddDynamic(
		this, &ThisClass::ComboBoxString_TasksClassification_OnSelectionChanged);
	ButtonAddTask->OnReleased.AddDynamic(this, &ThisClass::ButtonAddTaskOnClick);

	RegenerateTasksFromGlobalData();
	
	// Initialize day tracking and set timer for day change checks
	LastCheckedDay = MySaveGIS->GetDateTimeTodayTicks() / ETimespan::TicksPerDay;
	GetWorld()->GetTimerManager().SetTimer(DayCheckTimerHandle, this, &UUMG_TasksContainer::CheckForDayChange, 6.0f, true);
}

void UUMG_TasksContainer::CheckForDayChange()
{
	if (!MySaveGIS)
	{
		return;
	}
	{
		FString
			TempStr = FString::Printf(TEXT("Checking"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
	int64 CurrentDay = MySaveGIS->GetDateTimeTodayTicks() / ETimespan::TicksPerDay;
	
	// Check if the day has changed
	if (LastCheckedDay != CurrentDay && CurrentDay - LastCheckedDay > 0)
	{
		// Calculate days passed
		int32 DaysPassed = CurrentDay - LastCheckedDay;
		
		// Update LastCheckedDay
		LastCheckedDay = CurrentDay;
		
		// Notify SaveGIS about day change (it will be stored in AnotherDay)
		MySaveGIS->AnotherDay = DaysPassed;
		{
			FString
				TempStr = FString::Printf(TEXT("Generating"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}

		// Regenerate UI
		RegenerateTasksFromGlobalData();
		
		// Save changes
		MySaveGIS->SaveAllData();
	}
}

void UUMG_TasksContainer::ChangeChildrenSortname(UUMG_BasicTask* BasicTask, FText Sortname)
{
	if (BasicTask->GetVisibility() == ESlateVisibility::Collapsed)
	{
		return;
	}
	BasicTask->TaskData.SortName = Sortname.ToString();
}

UPanelSlot* UUMG_TasksContainer::MyInsertChildAt(int32 Index, UWidget* Content, UPanelWidget* ScrollBox)
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
