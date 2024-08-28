// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BFL_GetClasses.generated.h"

class UUMG_MainUI;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UBFL_GetClasses : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Utility", meta=(WorldContext="WorldContextObject"))
	static UUMG_MainUI* GetMainUI(UObject* WorldContextObject);

};
