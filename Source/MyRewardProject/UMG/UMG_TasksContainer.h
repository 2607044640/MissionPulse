// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG_TasksContainer.generated.h"

struct FTaskData;
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
	virtual void NativeConstruct() override;

public:
	
	void TaskFinish(FTaskData& TaskData, UUMG_BasicTask* Uumg_BasicTask);

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
