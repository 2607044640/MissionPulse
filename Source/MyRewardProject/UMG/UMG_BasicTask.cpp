// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_BasicTask.h"

#include "UMG_BasicEditer.h"
#include "UMG_MainUI.h"
#include "UMG_TasksContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MyRewardProject/Frameworks/MyHUD.h"


class IAssetRegistry;
class AMyHUD;

void UUMG_BasicTask::TaskFinish(UUMG_BasicTask* BasicTask)
{
	// Button_Finish->SetIsEnabled(false);
}

void UUMG_BasicTask::SlotScoreOnEditFinish(UUMG_BasicTask* InUumg_BasicTask, FText InText)
{
	TaskData.Score = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.Times = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotSavedTimesOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.SavedTimes = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotTitleOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.Title = InText.ToString();
}

void UUMG_BasicTask::SlotSavedDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.SavedDays = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::SlotDaysOnEditFinish(UUMG_BasicTask* Uumg_BasicTask, FText InText)
{
	TaskData.Days = FCString::Atoi(*InText.ToString());
}

void UUMG_BasicTask::ButtonClicked(UUMG_BasicTask* Uumg_BasicTask)
{
	UKismetSystemLibrary::K2_PauseTimerHandle(this, CheckPressedAddOrMinusHandle);
	if (TaskData.SavedTimes && IsAddTask)
	{
		if (OnAddScore.IsBound())
		{
			OnAddScore.Broadcast(this);
		}
	}
	else
	{
		if (OnMinusScore.IsBound())
		{
			OnMinusScore.Broadcast(this);
		}
	}


	CheckIfTaskFinish();

	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::AddScore(UUMG_BasicTask* BasicTask)
{
	TaskData.SavedTimes = TaskData.SavedTimes - 1;

	if (TaskData.bIsAddScore)
	{
		MySaveGIS->AddScore(TaskData.Score);
		BPOnAddScoreEffect();
	}
	else
	{
		MySaveGIS->MinusScore(TaskData.Score);
		BPOnMinusScoreEffect();
	}
}

void UUMG_BasicTask::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                       UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	//todo auto scroll up
}

void UUMG_BasicTask::MinusScore(UUMG_BasicTask* Uumg_BasicTask)
{
	IsAddTask = true;
	TaskData.SavedTimes = TaskData.SavedTimes + 1;
	if (TaskData.bIsAddScore)
	{
		MySaveGIS->MinusScore(TaskData.Score);
		BPOnMinusScoreEffect();
	}
	else
	{
		MySaveGIS->AddScore(TaskData.Score);
		BPOnAddScoreEffect();
	}
}

void UUMG_BasicTask::CheckPressedAddOrMinus()
{
	IsAddTask = false;
}

void UUMG_BasicTask::Button_FinishOnPressed()
{
	GetWorld()->GetTimerManager().SetTimer(CheckPressedAddOrMinusHandle, this, &UUMG_BasicTask::CheckPressedAddOrMinus,
	                                       CheckPressedAddOrMinusRate);
}

void UUMG_BasicTask::ButtonAddScoreOnClicked()
{
	int32 TempINT32 = FCString::Atoi(*SlotScore->TextBlock->GetText().ToString());
	TempINT32++;
	if (TempINT32 < 0)
	{
		TempINT32 = 0;
	}
	TaskData.Score = TempINT32;
	RefreshUI();
	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::ButtonMinusScoreOnClicked()
{
	int32 TempINT32 = FCString::Atoi(*SlotScore->TextBlock->GetText().ToString());
	TempINT32--;
	if (TempINT32 < 0)
	{
		TempINT32 = 0;
	}
	TaskData.Score = TempINT32;
	RefreshUI();
	MySaveGIS->SaveAllData();
}

void UUMG_BasicTask::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsCopiedWidget) return;

	//button
	Image_Coin->OnMouseButtonDownEvent.BindUFunction(this,TEXT("OnImageClicked"));

	Button_Finish->OnClicked.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnClicked);
	Button_Finish->OnPressed.AddDynamic(this, &UUMG_BasicTask::Button_FinishOnPressed);
	OnAddScore.AddUObject(this, &UUMG_BasicTask::AddScore);
	OnMinusScore.AddUObject(this, &UUMG_BasicTask::MinusScore);
	OnButtonClicked.AddUObject(this, &UUMG_BasicTask::ButtonClicked);
	ButtonAddScore->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonAddScoreOnClicked);
	ButtonMinusScore->OnClicked.AddDynamic(this, &UUMG_BasicTask::ButtonMinusScoreOnClicked);

	//finish
	OnTaskFinish.AddUObject(this, &UUMG_BasicTask::TaskFinish);
	AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	Parent_TasksContainer = MyHUD->MainUI->TasksContainer;
	if (!Parent_TasksContainer)
	{
		RemoveFromParent();
		FString teowjewo = FString::Printf(TEXT("None Parent_TasksContainer->RemoveFromParent"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, teowjewo, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *teowjewo);
	}

	OnTaskFinish.AddUObject(Parent_TasksContainer, &UUMG_TasksContainer::TaskFinish);
	OnTaskNotFinish.AddUObject(Parent_TasksContainer, &UUMG_TasksContainer::TaskNotFinish);

	//edit
	SlotTitle->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTitleOnEditFinish);
	SlotSavedTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedTimesOnEditFinish);
	SlotTimes->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotTimesOnEditFinish);
	SlotScore->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotScoreOnEditFinish);
	SlotSavedDays->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotSavedDaysOnEditFinish);
	SlotDays->OnEditFinish.AddUObject(this, &UUMG_BasicTask::SlotDaysOnEditFinish);

	//Other
	MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &UUMG_BasicTask::CheckIfTaskFinish, 0.2);

	RefreshUI();

	FMargin Margin(5, 0, 200, 0);
	SetPadding(Margin);
}


void UUMG_BasicTask::Button_FinishOnClicked()
{
	if (OnButtonClicked.IsBound())
	{
		OnButtonClicked.Broadcast(this);
	}
	RefreshUI();
}

void UUMG_BasicTask::CheckIfTaskFinish()
{
	if (!TaskData.SavedTimes)
	{
		if (OnTaskFinish.IsBound())
		{
			OnTaskFinish.Broadcast(this);
		}
	}
	else
	{
		if (OnTaskNotFinish.IsBound())
		{
			OnTaskNotFinish.Broadcast(this);
		}
	}
}

FReply UUMG_BasicTask::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

UUMG_BasicTask* UUMG_BasicTask::CopySelf()
{
	if (auto ThisWidget = CreateWidget<UUMG_BasicTask>(GetOwningPlayer(), Parent_TasksContainer->UIClass))
	{
		ThisWidget->IsCopiedWidget = true;
		ThisWidget->TaskData = TaskData;
		ThisWidget->RefreshUI();

		return ThisWidget;
	}

	FString ewg = FString::Printf(TEXT("CopySelf Not Valid"));
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, ewg, false);
	UE_LOG(LogTemp, Error, TEXT("%s"), *ewg);

	return nullptr;
}

void UUMG_BasicTask::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                          UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass());
	OutOperation->Payload = this;
	OutOperation->DefaultDragVisual = this;
	OutOperation->Pivot = EDragPivot::MouseDown;

	RemoveFromParent();
}

bool UUMG_BasicTask::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                  UDragDropOperation* InOperation)
{
	if (UUMG_BasicTask* OtherBasicTask = Cast<UUMG_BasicTask>(InOperation->Payload))
	{
		//Check Which ScrollBox should be

		UScrollBox* TempScrollBox = Parent_TasksContainer->ScrollBox_Tasks_Finish;
		if (Parent_TasksContainer->ScrollBox_Tasks->HasChild(this))
		{
			TempScrollBox = Parent_TasksContainer->ScrollBox_Tasks;
		}


		//Check Higher Or Lower
		FGeometry CachedGeometry = GetCachedGeometry();
		float ScreenPosition = CachedGeometry.GetAbsolutePosition().Y;

		FVector2D MousePosition = InDragDropEvent.GetScreenSpacePosition() - OtherBasicTask->GetDesiredSize() / 2;


		float OtherScreenPosition = MousePosition.Y;

		//Set Higher Or Lower By Index
		int32 TempIndexfloat = TempScrollBox->GetChildIndex(this);

		int32 TempIndex = ScreenPosition > OtherScreenPosition ? TempIndexfloat : TempIndexfloat + 1;
		TempScrollBox->InsertChildAt(TempIndex, OtherBasicTask);


		// 获取所有子控件
		TArray<UWidget*> Children = TempScrollBox->GetAllChildren();

		//todo 这里可能写成一个Static Function
		// 按照 ChildIndex 进行排序
		Children.Sort([TempScrollBox](const UWidget& A, const UWidget& B)
		{
			int32 IndexA = TempScrollBox->GetChildIndex(&A);
			int32 IndexB = TempScrollBox->GetChildIndex(&B);
			return IndexA < IndexB;
		});


		// 清空 ScrollBox 并重新添加子控件
		TempScrollBox->ClearChildren();
		for (UWidget* Child : Children)
		{
			TempScrollBox->AddChild(Child);
		}

		MySaveGIS->SaveAllData();
	}

	return false;
}

void UUMG_BasicTask::RefreshUI()
{
	SlotTitle->TextBlock->SetText(FText::FromString(TaskData.Title));
	SlotSavedTimes->TextBlock->SetText(FText::AsNumber(TaskData.SavedTimes));
	SlotTimes->TextBlock->SetText(FText::AsNumber(TaskData.Times));
	SlotDays->TextBlock->SetText(FText::AsNumber(TaskData.Days));
	SlotSavedDays->TextBlock->SetText(FText::AsNumber(TaskData.SavedDays));
	SlotScore->TextBlock->SetText(FText::AsNumber(TaskData.Score));

	RefreshImage();
	// AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
	// MyHUD->MainUI->TasksContainer->SlotTotalScore->TextBlock->SetText(FText::AsNumber(MySaveGIS->GetScore()));

	BPOtherRefresh();
}


void UUMG_BasicTask::RefreshImage()
{
	if (TaskData.bIsAddScore)
	{
		Image_Coin->SetColorAndOpacity(GoldColor);
	}
	else
	{
		Image_Coin->SetColorAndOpacity(RedGoldColor);
	}
}

FEventReply UUMG_BasicTask::OnImageClicked(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	TaskData.bIsAddScore = !TaskData.bIsAddScore;

	RefreshImage();

	MySaveGIS->SaveAllData();

	FEventReply TempEvent;
	return TempEvent;
}
