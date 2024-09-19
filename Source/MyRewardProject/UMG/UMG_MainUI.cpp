// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_MainUI.h"

#include "UMG_BasicTask.h"
#include "Blueprint/DragDropOperation.h"

void UUMG_MainUI::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UUMG_MainUI::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                               UDragDropOperation* InOperation)
{
	if (UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(InOperation->Payload))
	{
		if (UMG_BasicTask->OnBasicTaskUnselected.IsBound())
		{
			UMG_BasicTask->OnBasicTaskUnselected.Broadcast();
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
