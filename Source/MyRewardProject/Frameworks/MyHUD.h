// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyRewardProject/UMG/UMG_MainUI.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API AMyHUD : public AHUD
{
	GENERATED_BODY()

	template <class T>
	void MakeUserWidget(T*& ObjectPtr, TSubclassOf<T> UIClass);
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	TSubclassOf<UUMG_MainUI> MainUIClass;
	UPROPERTY(BlueprintReadOnly, Category=JFSetting)
	UUMG_MainUI* MainUI;
};


