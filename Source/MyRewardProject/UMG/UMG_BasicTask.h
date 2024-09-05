// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG_TasksContainer.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_BasicTask.generated.h"

class UUMG_BasicEditer;
struct FTaskData;

class UUMG_BasicTask;
DECLARE_MULTICAST_DELEGATE_OneParam(TaskStateChanged, UUMG_BasicTask* BasicTask)

class UButton;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_BasicTask : public UUserWidget
{
public:
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
	TaskStateChanged OnAddScore;
	TaskStateChanged OnMinusScore;
	TaskStateChanged OnTaskFinish;
	TaskStateChanged OnTaskNotFinish;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FTaskData TaskData;
	

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
	;
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
	bool bTaskIsAddScore = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bIsAddTask = true;
	FTimerHandle CheckPressedAddOrMinusHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float CheckPressedAddOrMinusRate = 0.5;

	UPROPERTY()
	UMySaveGIS* MySaveGIS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UUMG_TasksContainer* ParentTasksContainer;



	//Init
	void TaskFinish(UUMG_BasicTask* BasicTask);

	void SlotScoreOnEditFinish(UUMG_BasicTask* InUumg_BasicTask, FText InText);
	void SlotTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotSavedTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	UFUNCTION()
	void SlotTitleOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotSavedDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void SlotDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void ButtonClicked(UUMG_BasicTask* Uumg_BasicTask);
	void MinusScore(UUMG_BasicTask* Uumg_BasicTask);
	void CheckPressedAddOrMinus();
	UFUNCTION()
	void Button_FinishOnPressed();
	UFUNCTION()
	void ButtonAddScoreOnClicked();
	UFUNCTION()
	void ButtonMinusScoreOnClicked();
	
	virtual void NativeConstruct() override;
	UFUNCTION()
	void Button_FinishOnClicked();
	void CheckIfTaskFinish();
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	UUMG_BasicTask* CopySelf();
	
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
									  UDragDropOperation*& OutOperation) override;
	
	void AddScore(UUMG_BasicTask* BasicTask);
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	GENERATED_BODY()
};
