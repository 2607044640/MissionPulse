// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "MyRewardProject.h"
#include "GameInstanceSubsystems/MySaveGIS.h"

void UMyGameInstance::Init()
{
	Super::Init();
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown(); //Put super shutdown at end
}
