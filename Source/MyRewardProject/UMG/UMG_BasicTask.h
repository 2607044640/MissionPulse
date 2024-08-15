// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_BasicTask.generated.h"

class UUMG_BasicEditer;
struct FTaskData;
class UTextBlock;
class UUMG_BasicTask;
DECLARE_MULTICAST_DELEGATE_TwoParams(TaskStateChanged, FTaskData& TaskData, UUMG_BasicTask* BasicTask)

class UButton;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_BasicTask : public UUserWidget
{
	void TaskFinish(FTaskData& InTaskData, UUMG_BasicTask* BasicTask);

	void SlotScoreOnEditFinish(FTaskData& InTaskData, UUMG_BasicTask* InUumg_BasicTask);
	virtual void NativeConstruct() override;
	UFUNCTION()
	void Button_FinishOnClicked();

	void AddScore(FTaskData& InTaskData, UUMG_BasicTask* BasicTask);

public:
	void RefreshUI();

	UFUNCTION(BlueprintImplementableEvent)
	void BPOnAddScoreEffect();

	TaskStateChanged OnAddScore;
	TaskStateChanged OnMinusScore;
	TaskStateChanged OnTaskFinish;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FTaskData TaskData;

public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_Finish;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotTitle;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotSavedTimes;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotTimes;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotDays;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* SlotScore;
	GENERATED_BODY()
};
