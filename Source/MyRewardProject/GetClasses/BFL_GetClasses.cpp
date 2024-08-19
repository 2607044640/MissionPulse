// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_GetClasses.h"

#include "Kismet/GameplayStatics.h"
#include "MyRewardProject/Frameworks/MyHUD.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"

class AMyHUD;

UUMG_MainUI* UBFL_GetClasses::GetMainUI(UObject* WorldContextObject)
{
	AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(WorldContextObject, 0)->GetHUD());
	return MyHUD->MainUI;
}
