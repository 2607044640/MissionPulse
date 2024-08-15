// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_BasicEditer.generated.h"

class UTextBlock;
class UEditableTextBox;
class UUMG_BasicTask;

DECLARE_MULTICAST_DELEGATE_TwoParams(TaskStateChanged, FTaskData& TaskData, UUMG_BasicTask* BasicTask)
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_BasicEditer : public UUserWidget
{
	UFUNCTION()
	void EditableTextBox_BasicOnTextChanged(const FText& Text);
	UFUNCTION()
	void EditableTextBox_BasicOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FTaskData TaskData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UUMG_BasicTask* BasicTask;

	TaskStateChanged OnEditFinish;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_Basic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bIsNumeric = true;
	GENERATED_BODY()
};
