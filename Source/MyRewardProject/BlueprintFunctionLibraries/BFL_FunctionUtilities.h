// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_FunctionUtilities.generated.h"

class UWidget;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UBFL_FunctionUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	static AActor* SpawnActorAtWidgetWorldPosition(UWidget* Widget, TSubclassOf<AActor> ActorToSpawn,
	                                     APlayerController* PlayerController);
};
