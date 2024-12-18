// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_FunctionUtilities.generated.h"

class UWidget;

/**
 * 
 */
UCLASS()
class UMG_UTILITIES_API UBFL_FunctionUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Utility", meta=(WorldContext="WorldContextObject"))
	static bool CheckMouseIsInsideOfWidget_DragDropEvent(UObject* WorldContextObject, UWidget* ChildWidget, FVector2D InDragDropEventGetScreenSpacePosition);
	UFUNCTION(BlueprintCallable, Category="UI",meta=(Keywords="NumberIncrease,NumberDecrease,ChangeNumber"))
	static float JFNumberDeOrIncreaseGradually(float Number_UI, float NumberToReach, float Speed = 3.f,
	                                           float LessThan = 0.2f);

	UFUNCTION(BlueprintCallable, Category="UI", meta=(WorldContext="WorldContextObject"),meta=(Keywords="CreateWidget"))
	static UUserWidget* JFSetWidgetPositionByDPI(UObject* WorldContextObject, UWidget* WidgetToGetPos,
	                                             TSubclassOf<UUserWidget> WidgetClassToCreate,
	                                             UUserWidget* ExistWidget_Optional);
	UFUNCTION(BlueprintCallable, Category="UI", meta=(WorldContext="WorldContextObject"))
	static UUserWidget* JFLerpWidgetToAnotherWidget(UObject* WorldContextObject, UUserWidget* WidgetToDoLerp,
	                                                UWidget* StartWidget, UWidget* EndWidget,
	                                                float LerpValue);
	UFUNCTION(BlueprintCallable, Category="UI")
	static void SortPanelWidgetsChildren(UPanelWidget* InPanelWidget);

	UFUNCTION(BlueprintCallable, Category="UI", meta=(WorldContext="WorldContextObject"))
	static UUserWidget* JF2LerpWidgetWithRandomToAnotherWidget(UObject* WorldContextObject, UUserWidget* WidgetToDoLerp,
	                                                           UWidget* StartWidget, UWidget* EndWidget,
	                                                           float LerpValue, FVector2D RandomPosition);
	UFUNCTION(BlueprintCallable, Category = "UI", meta=(WorldContext="WorldContextObject"))
	static AActor* JFSpawnActorAtWidgetWorldPosition(UObject* WorldContextObject, UWidget* Widget,
	                                                 TSubclassOf<AActor> ActorToSpawn,
	                                                 APlayerController* PlayerController);
	UFUNCTION(BlueprintCallable, Category="Utility", BlueprintPure)
	static FText JFFloatToText(const float& InputFloat);

	UFUNCTION(BlueprintCallable, Category = "UI", meta=(WorldContext="WorldContextObject"))
	static FVector2D JFGetWidgetViewPortPosition(UObject* WorldContextObject,UWidget* Widget);
};
