// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UMyGameInstance : public UGameInstance
{
	virtual void Init() override;
	virtual void Shutdown() override;
	GENERATED_BODY()
};
