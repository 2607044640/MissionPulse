// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicEditer.h"

#include "UMG_BasicTask.h"
#include "Components/EditableTextBox.h"
#include "Components/PanelWidget.h"

void UUMG_BasicEditer::EditableTextBox_BasicOnTextChanged(const FText& Text)
{
	if (bIsNumeric)
	{
		FString Input = Text.ToString();
		FString FilteredText;

		for (TCHAR Char : Input)
		{
			if (FChar::IsDigit(Char))
			{
				FilteredText.AppendChar(Char);
			}
		}

		EditableTextBox_Basic->SetText(FText::FromString(FilteredText));
	}
}

void UUMG_BasicEditer::EditableTextBox_BasicOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (OnEditFinish.IsBound() && TaskData.MyStructIsValid() && BasicTask)
	{
		OnEditFinish.Broadcast(TaskData, BasicTask);
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("NotValid: TaskData.MyStructIsValid()&&BasicTask"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}

void UUMG_BasicEditer::NativeConstruct()
{
	Super::NativeConstruct();
	EditableTextBox_Basic->OnTextChanged.AddDynamic(this, &UUMG_BasicEditer::EditableTextBox_BasicOnTextChanged);
	EditableTextBox_Basic->OnTextCommitted.AddDynamic(this, &UUMG_BasicEditer::EditableTextBox_BasicOnTextCommitted);

	//get data from parent
	UUMG_BasicTask* OwningWidget = GetParent()->GetTypedOuter<UUMG_BasicTask>();
	BasicTask = OwningWidget;
	TaskData = OwningWidget->TaskData;
}