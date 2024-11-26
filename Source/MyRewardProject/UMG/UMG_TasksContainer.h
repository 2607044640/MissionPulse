// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "UMG_TasksContainer.generated.h"

struct FTaskData;
class UMySaveGIS;
class UImage;
class UComboBoxString;
class UUMG_BasicEditer;
class UScrollBox;
class UUMG_BasicTask;
class UButton;
class UTextBlock;
/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(OnMouseButtonEvent)


UCLASS()
class MYREWARDPROJECT_API UUMG_TasksContainer : public UUserWidget
{
	template <typename Func>
	void ExecuteForAllChildrenWithLambda(Func Function);

	// template <class TClass>
	// void ExecuteForAllChildrenWithStdFunction(UScrollBox* ScrollBox, std::function<void(TClass*)> Func);
	template <class TClass>
	void ExecuteForAllChildrenWithStdFunction(std::function<void(TClass*)> Func);
	UFUNCTION(BlueprintCallable)
	void ButtonAddTaskOnClick();
	
	void SetVisibilityWhenSelectionChanged(UUMG_BasicTask* UMG_BasicTask, FString SelectedItem);
	
	UFUNCTION()
	void ComboBoxString_TasksClassification_OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	void TaskDataTransformToTask(FTaskData InTaskData);

	void BasicEditer_GlobalDailyProgressOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText Text);
	void BasicEditer_DailyProgressRewardValueOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText Text);
	UFUNCTION(BlueprintCallable)
	void ClearThenGenerateSortedOptions();

	template <class TClass, class TMemberFunc, class... TArgs>
	void ExecuteFP_OperateChildren(TClass* Instance, TMemberFunc Func, TArgs&&... Args);
	void ChangeOption();


	UFUNCTION()
	void EditableTextBox_SortNameOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void Button_AddSortNameOnClicked();
	UFUNCTION()
	void Button_ChangeSortNamesOnClicked();
	UFUNCTION()
	void ButtonChangeSortName_TaskOnClick();
	virtual void NativeConstruct() override;
	bool bIsChangeSortName_Task;
	UPROPERTY()
	UUMG_BasicTask* TempSelectedBasicTask;
	FString TempStringRecorderForSelection;

public:
	UFUNCTION(BlueprintCallable)
	void RemoveChildrenThatVisible();
	UFUNCTION(BlueprintCallable)
	void RegenerateTasksFromGlobalData();
	OnMouseButtonEvent TaskContainerOnMouseButtonDown;
	UFUNCTION(BlueprintImplementableEvent)
	void BPAddOption();
	bool bIsAddOption;
	void ChangeChildrenSortname(UUMG_BasicTask* BasicTask, FText Sortname);

	UFUNCTION(BlueprintImplementableEvent)
	void BPOnDailyProgressEditFinish();
	UFUNCTION(BlueprintCallable)

	UPanelSlot* MyInsertChildAt(int32 Index, UWidget* Content, UPanelWidget* ScrollBox);

	int32 CalcAndGetIndex(FVector2D MousePosition, UPanelWidget* InPanelWidget);

	void TaskNotFinish(UUMG_BasicTask* Uumg_BasicTask);
	void TaskFinish(UUMG_BasicTask* Uumg_BasicTask);
	void BroadcastBasicTaskUnselected(UUMG_BasicTask* UMG_BasicTask);
	void RemoveAllSelectedBasicTask();
	UFUNCTION(BlueprintImplementableEvent)
	void OnFinishDailyProgress();

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UComboBoxString* ComboBoxString_TasksClassification;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock_Score;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_SortName;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UButton* Button_ChangeSortNames;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_AddSortName;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UScrollBox* ScrollBox_Tasks;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UScrollBox* ScrollBox_Tasks_Finish;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_GlobalDailyProgress;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_DailyProgressRewardValue;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock_GlobalDailyProgress_Saved;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* ButtonAddTask;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UUMG_BasicTask> UIClass;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY()
	UMySaveGIS* MySaveGIS;
	double ScrollBoxOffset_Finish;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	double SpeedOfScroll = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	double OffsetOfScroll = 110;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* ButtonChangeSortName_Task;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UUMG_BasicTask* SelectedBasicTask;

	double SpeedOfScroll_ByEdgeDistance = 1;

	UFUNCTION(BlueprintCallable)
	void ScrollTheChildDown(bool IsDown, UWidget* InBasicTask);
	GENERATED_BODY()
};
