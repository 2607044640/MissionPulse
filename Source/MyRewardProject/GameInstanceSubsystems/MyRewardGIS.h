// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyRewardGIS.generated.h"

/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UMyRewardGIS : public UGameInstanceSubsystem
{
	float Score;

public:
	UFUNCTION(BlueprintCallable)
	void AddScore(float AddNum);
	UFUNCTION(BlueprintCallable)
	void MinusScore(float MinusNum);
	GENERATED_BODY()
};
