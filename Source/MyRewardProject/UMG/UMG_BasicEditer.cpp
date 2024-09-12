// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicEditer.h"
#include "UMG_BasicTask.h"
#include "UMG_MainUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Components/EditableTextBox.h"
#include "Components/PanelWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "MyRewardProject/Frameworks/MyHUD.h"

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
	if (OnEditFinish.IsBound())
	{
		OnEditFinish.Broadcast(BasicTask, EditableTextBox_Basic->GetText());
		TextBlock->SetText(EditableTextBox_Basic->GetText());
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("NotValid: TaskData.MyStructIsValid()&&BasicTask"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}

void UUMG_BasicEditer::ThisOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	if (WidgetSwitcher->GetActiveWidget() == Button)return;
	
	WidgetSwitcher->SetActiveWidget(Button);
	UMySaveGIS* MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
	MySaveGIS->SaveAllData();
}

void UUMG_BasicEditer::TaskContainerOnMouseButtonDownFunc()
{
	if (OnEditFinish.IsBound())
	{
		if (WidgetSwitcher->GetActiveWidget() == Button)return;
		OnEditFinish.Broadcast(BasicTask, EditableTextBox_Basic->GetText());
		TextBlock->SetText(EditableTextBox_Basic->GetText());
	}
	else
	{
		FString TempStr = FString::Printf(TEXT("NotValid: TaskData.MyStructIsValid()&&BasicTask"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
	}
}

void UUMG_BasicEditer::ButtonOnClicked()
{
	if (TaskContainer->TaskContainerOnMouseButtonDown.IsBound())
	{
		TaskContainer->TaskContainerOnMouseButtonDown.Broadcast();
	}
	WidgetSwitcher->SetActiveWidget(EditableTextBox_Basic);
	EditableTextBox_Basic->SetText(TextBlock->GetText());
}

void UUMG_BasicEditer::NativeConstruct()
{
	Super::NativeConstruct();
	if (OnEditFinish.IsBound())
	{
		return;
	}
	//bind
	Button->OnClicked.AddDynamic(this, &UUMG_BasicEditer::ButtonOnClicked);

	EditableTextBox_Basic->OnTextChanged.AddDynamic(this, &UUMG_BasicEditer::EditableTextBox_BasicOnTextChanged);
	EditableTextBox_Basic->OnTextCommitted.AddDynamic(this, &UUMG_BasicEditer::EditableTextBox_BasicOnTextCommitted);
	AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	TaskContainer = MyHUD->MainUI->TasksContainer;

	TaskContainer->TaskContainerOnMouseButtonDown.AddUObject(
		this, &UUMG_BasicEditer::TaskContainerOnMouseButtonDownFunc);

	OnEditFinish.AddUObject(this, &UUMG_BasicEditer::ThisOnEditFinish);

	//get data from parent
	if (UUMG_BasicTask* OwningWidget = GetParent()->GetTypedOuter<UUMG_BasicTask>())
	{
		BasicTask = OwningWidget;
	}
}
