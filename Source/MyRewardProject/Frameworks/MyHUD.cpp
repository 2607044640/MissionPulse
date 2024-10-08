// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "MyRewardProject/UMG/UMG_MainUI.h"

template <typename T>
void AMyHUD::MakeUserWidget(T*& ObjectPtr, TSubclassOf<T> UIClass)
{
	if (!ObjectPtr)
	{
		if (UIClass)
		{
			ObjectPtr = CreateWidget<T>(GetOwningPlayerController(), UIClass);
			if (ObjectPtr)
			{
				ObjectPtr->AddToViewport();
			}
		}
		else
		{
			FString TempStr = FString::Printf(TEXT("No specific class specified at : %s"), *GetClass()->GetName());
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		}
	}
}

void AMyHUD::BeginPlay()
{
	MakeUserWidget<UUMG_MainUI>(MainUI, MainUIClass);
	Super::BeginPlay();
}
