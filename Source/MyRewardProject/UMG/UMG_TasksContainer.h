// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_TasksContainer.generated.h"

class UComboBoxString;
class UUMG_BasicEditer;
class UScrollBox;
class UUMG_BasicTask;
class UButton;
class UTextBlock;
/**
 * 
 */

UCLASS()
class MYREWARDPROJECT_API UUMG_TasksContainer : public UUserWidget
{
	UFUNCTION()
	void ButtonAddTaskOnClick();
	UFUNCTION()
	void ComboBoxString_TasksClassification_OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	void TaskDataAddToTask(FTaskData InTaskData);
	virtual void NativeConstruct() override;

public:

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BPDoTest();

	
	UFUNCTION(BlueprintCallable)
	float NumberDeOrIncreaseGradually(float Number, float SavedNumber, float Speed=3.f, float LessThan = 0.2f);
	TArray<FString> Global_SortNames;


	void TaskNotFinish(UUMG_BasicTask* Uumg_BasicTask);
	void TaskFinish(UUMG_BasicTask* Uumg_BasicTask);
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UComboBoxString* ComboBoxString_TasksClassification;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock_Score;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Button_Back;
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UScrollBox* ScrollBox_Tasks;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UScrollBox* ScrollBox_Tasks_Finish;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* ButtonAddTask;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UUMG_BasicTask> UIClass;

	GENERATED_BODY()
};
