// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG_MainUI.generated.h"

class UUMG_TasksContainer;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_MainUI : public UUserWidget
{
	virtual void NativeConstruct() override;
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_TasksContainer* TasksContainer;
	
	GENERATED_BODY()
};
