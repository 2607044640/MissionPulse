// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_BasicTask.generated.h"

struct FTaskData;
class UTextBlock;
class UUMG_BasicTask;
DECLARE_MULTICAST_DELEGATE_TwoParams(TaskStateChanged, FTaskData& TaskData,UUMG_BasicTask* BasicTask)

class UButton;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_BasicTask : public UUserWidget
{
	void TaskFinish(FTaskData& InTaskData,UUMG_BasicTask* BasicTask);
	
	UFUNCTION()
	void Button_EditTaskOnClick();
	virtual void NativeConstruct() override;
	UFUNCTION()
	void Button_FinishOnClicked();

	void AddScore(FTaskData& InTaskData,UUMG_BasicTask* BasicTask);

public:
	void RefreshUI();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BPTaskFinishEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void BPOnAddScoreEffect();

	TaskStateChanged OnAddScore;
	TaskStateChanged OnMinusScore;
	TaskStateChanged OnTaskFinish;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FTaskData TaskData;

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_EditTask;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UButton* Button_Finish;
	
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* SlotTitle;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* SlotTimes;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* SlotDays;
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UTextBlock* SlotScore;
	GENERATED_BODY()
};
