// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_BasicTask.generated.h"


class UUMG_TasksContainer;
class UImage;
class UBorder;
class UUMG_BasicEditer;
class UUMG_BasicTask;
class UMySaveGIS;
struct FTaskData;
class UButton;
class MySaveGIS;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBasicTaskSelect);
DECLARE_MULTICAST_DELEGATE_OneParam(TaskStateChanged, UUMG_BasicTask* BasicTask)
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_BasicTask : public UUserWidget
{
public:
	UFUNCTION(BlueprintImplementableEvent)
	void BPTaskOnUnSelectedHideTimesAndDaysUMG();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FTaskData TaskData;
	
	UPROPERTY(BlueprintAssignable)
	FOnBasicTaskSelect OnBasicTaskSelected;
	UPROPERTY(BlueprintAssignable)
	FOnBasicTaskSelect OnBasicTaskUnselected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FLinearColor BP_Border_UserVisualColor_Color;
	FLinearColor PreviousColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FMargin BasicTaskMargin;

	UFUNCTION(BlueprintImplementableEvent)
	void BPOtherRefresh();
	UFUNCTION(BlueprintCallable)
	void RefreshUI();
	void RefreshImage();

	UFUNCTION(BlueprintImplementableEvent)
	void BPOnMinusScoreEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void BPOnAddScoreEffect();
	

	TaskStateChanged OnButtonClicked;
	TaskStateChanged OnMinusScore;
	TaskStateChanged OnAddScore;
	TaskStateChanged OnTaskFinish;
	TaskStateChanged OnTaskNotFinish;

	

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UImage* Image_Coin;
	FLinearColor SavedColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FLinearColor GoldColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FLinearColor RedGoldColor;
	bool bIsCopiedWidget;

	UFUNCTION()
	FEventReply OnImageClicked(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* ButtonAddScore;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* ButtonMinusScore;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_Finish;

	//Info
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotTitle;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotSavedTimes;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotTimes;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotDays;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotSavedDays;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotScore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bMyTaskIsAddScore = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bBasicTaskIsAddTask = true;
	FTimerHandle CheckPressedAddOrMinusHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float CheckPressedAddOrMinusRate = 0.5;

	UPROPERTY()
	UMySaveGIS* MySaveGIS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UUMG_TasksContainer* umg_ParentTasksContainer;


	//Init
	void TaskFinish(UUMG_BasicTask* BasicTask);

	void SlotScoreOnEditFinish(UUMG_BasicTask* InUumg_BasicTask, FText InText);
	void SlotTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotSavedTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotTitleOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotSavedDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void ButtonClicked(UUMG_BasicTask* Uumg_BasicTask);
	void SubTaskSavedTimes(UUMG_BasicTask* Uumg_BasicTask);
	void CheckPressedAddOrMinus();
	UFUNCTION()
	void Button_FinishOnPressed();
	UFUNCTION()
	void ButtonAddScoreOnClicked();
	UFUNCTION()
	void ButtonMinusScoreOnClicked();

	UFUNCTION()
	void ButtonSelectOnClick();
	virtual void NativeConstruct() override;
	UFUNCTION()
	void Button_FinishOnClicked();
	void CheckIfTaskFinish();
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* ButtonSelect;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	                          UDragDropOperation* InOperation) override;
	// UUMG_BasicTask* CopySelf();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UBorder* Border_UserVisualColor;
	UFUNCTION()
	void TaskOnSelected();
	UFUNCTION()
	void TaskOnUnSelected();
	void AddTaskSavedTimes(UUMG_BasicTask* BasicTask);

	GENERATED_BODY()
};