// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "Kismet/GameplayStatics.h"
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

void AMyHUD::OnWindowFocusChanged(bool bIsFocused)
{
	// Put code below into BeginPlay
	// FSlateApplication::Get().OnApplicationActivationStateChanged()
	// .AddUObject(this, &AMyHUD::OnWindowFocusChanged);

	// Don't pause in the editor, it's annoying
#if !WITH_EDITOR
	//In this area, There's no compilation check. Be careful not to add more logic.Otherwise, packaging will be failed
	if ( bIsFocused )
	{
		// FString TempStr = FString::Printf(TEXT("Focus"));
		// if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		// UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		// Unlimit game FPS
		GEngine->SetMaxFPS( 0 );

		// Unpause the game
		UGameplayStatics::SetGamePaused(this,false);
	}
	else
	{
		// FString TempStr = FString::Printf(TEXT("NotFocus"));
		// if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		// UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		// Reduce FPS to max 10 while in the background
		GEngine->SetMaxFPS( 10.0f );

		// Pause the game, using your "show pause menu" function
		UGameplayStatics::SetGamePaused(this,true);
	}
#endif
}

bool AMyHUD::GetIsGameWindowFocused()
{
	if (GetWorld())
	{
		UGameViewportClient* vpClient = GetWorld()->GetGameViewport();
		if (vpClient)
		{
			FViewport* vp = vpClient->Viewport;
			if (vp)
			{
				return vp->IsForegroundWindow();
			}
		}
	}
	return false;
}


void AMyHUD::BeginPlay()
{
	MakeUserWidget<UUMG_MainUI>(MainUI, MainUIClass);

	FSlateApplication::Get().OnApplicationActivationStateChanged()
	                        .AddUObject(this, &AMyHUD::OnWindowFocusChanged);
	
	Super::BeginPlay();
}
