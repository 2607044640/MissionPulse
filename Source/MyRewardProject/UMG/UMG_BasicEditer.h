// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG_TasksContainer.h"
#include "UMG_BasicEditer.generated.h"

class UWidgetSwitcher;
class UButton;
class UTextBlock;
class UEditableTextBox;
class UUMG_BasicTask;


DECLARE_MULTICAST_DELEGATE_TwoParams(OnTaskEditedChanged, UUMG_BasicTask* BasicTask, FText InText)

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

	void ThisOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText);
	void TaskContainerOnMouseButtonDownFunc();

	UFUNCTION()
	void ButtonOnClicked();
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UButton* Button;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UWidgetSwitcher* WidgetSwitcher;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	UUMG_BasicTask* BasicTask;
	UPROPERTY()
	UUMG_TasksContainer* TaskContainer;


	OnTaskEditedChanged OnEditFinish;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UEditableTextBox* EditableTextBox_Basic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	bool bIsNumeric = true;
	GENERATED_BODY()
};
