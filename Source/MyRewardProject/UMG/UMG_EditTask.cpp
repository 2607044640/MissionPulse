// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_EditTask.h"

#include "UMG_BasicTask.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UUMG_EditTask::EditableTextBox_ScoreOnTextChanged(const FText& Text)
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

	EditableTextBox_Score->SetText(FText::FromString(FilteredText));
}

void UUMG_EditTask::Button_AcceptOnClick()
{
	if (!TaskData.MyStructIsValid())
	{
		AddNewBasicTaskToEditTask();
	}

	if (OnEditedFinish.IsBound())
	{
		OnEditedFinish.Broadcast(TaskData, Uumg_BasicTask);
	}
}

void UUMG_EditTask::EditedFinish(FTaskData& InTaskData, UUMG_BasicTask* InUumg_BasicTask)
{
	//内容复制到
	InTaskData.Score = FCString::Atoi(*EditableTextBox_Score->GetText().ToString());
	InTaskData.Days = FCString::Atoi(*EditableTextBox_Days->GetText().ToString());
	InTaskData.Times = FCString::Atoi(*EditableTextBox_Times->GetText().ToString());
	InTaskData.SavedTimes = FCString::Atoi(*EditableTextBox_Times->GetText().ToString());

	InTaskData.Title = EditableTextBox_Title->GetText().ToString();

	InUumg_BasicTask->TaskData = InTaskData;

	InUumg_BasicTask->RefreshUI();

	RemoveFromParent();
}

void UUMG_EditTask::NativeConstruct()
{
	Super::NativeConstruct();

	OnEditedFinish.AddUObject(this, &UUMG_EditTask::EditedFinish);

	Button_Accept->OnClicked.AddDynamic(this, &UUMG_EditTask::Button_AcceptOnClick);
	EditableTextBox_Score->OnTextChanged.AddDynamic(this, &UUMG_EditTask::EditableTextBox_ScoreOnTextChanged);

	if (TaskData.MyStructIsValid())
	{
		EditableTextBox_Score->SetText(FText::AsNumber(TaskData.Score));
		EditableTextBox_Days->SetText(FText::AsNumber(TaskData.Days));
		EditableTextBox_Times->SetText(FText::AsNumber(TaskData.Times));

		EditableTextBox_Title->SetText(FText::FromString(TaskData.Title));
	}
}
