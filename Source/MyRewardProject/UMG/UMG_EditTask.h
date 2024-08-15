// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_EditTask.generated.h"

class UUMG_BasicTask;
DECLARE_MULTICAST_DELEGATE_TwoParams(TaskStateChanged, FTaskData& TaskData,UUMG_BasicTask* BasicTask)

class UButton;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_EditTask : public UUserWidget
{
	UFUNCTION()
	void EditableTextBox_ScoreOnTextChanged(const FText& Text);

	UFUNCTION()
	void Button_AcceptOnClick();
	void EditedFinish(FTaskData& InTaskData, UUMG_BasicTask* InUumg_BasicTask);
	virtual void NativeConstruct() override;

public:
	
	TaskStateChanged OnEditedFinish;

	UFUNCTION(BlueprintImplementableEvent)
	void AddNewBasicTaskToEditTask();
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_Score;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_Title;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_Times;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_Days;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting, meta = (ExposeOnSpawn="true"))
	FTaskData TaskData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting, meta = (ExposeOnSpawn="true"))
	UUMG_BasicTask* Uumg_BasicTask;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_Accept;
	GENERATED_BODY()
};
