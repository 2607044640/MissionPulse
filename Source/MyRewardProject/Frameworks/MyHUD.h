// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UUMG_MainUI;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
	template <class T>
	void MakeUserWidget(T*& ObjectPtr, TSubclassOf<T> UIClass);
	void OnWindowFocusChanged(bool bIsFocused);
	bool GetIsGameWindowFocused();
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UUMG_MainUI> MainUIClass;
	UPROPERTY(BlueprintReadOnly, Category=JFSetting)
	UUMG_MainUI* MainUI;
};


