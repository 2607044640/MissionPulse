// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MySaveGIS.generated.h"

USTRUCT(BlueprintType)
struct FTaskData
{
	UPROPERTY(EditAnywhere, Category=JFSetting)
	FString SortName;

	UPROPERTY(EditAnywhere, Category=JFSetting)
	FString Title;
	UPROPERTY(EditAnywhere, Category=JFSetting)
	FString Detail;

	UPROPERTY(EditAnywhere, Category=JFSetting)
	float Score;

	UPROPERTY(EditAnywhere, Category=JFSetting)
	int32 Days; // == 0 -> IsOnce
	UPROPERTY(EditAnywhere, Category=JFSetting)
	int32 Times;

	UPROPERTY(EditAnywhere, Category=JFSetting)
	int32 SavedTimes; // == 0 -> IsFinish

	FTaskData():Days(INDEX_NONE)
	{
		Score = 1;
		// Days = 0;
		Times = 1;
	}

	bool MyStructIsValid() const
	{
		return Days != INDEX_NONE;
	}

	
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct FRewardPerDays //todo 模仿原神或者皇室的那个奖励令牌系统，还有每日任务奖励系统，日活要达标
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category=JFSetting)
	float DailyBonusPoints;
	UPROPERTY(EditAnywhere, Category=JFSetting)
	float DailyBonusPointsRewardValue;
};

USTRUCT(BlueprintType)
struct FGlobalData //todo Add to AllDataToSave
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category=JFSetting)
	float TotalScore;
};


USTRUCT(BlueprintType)
struct FAllDataToSave
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	TArray<FTaskData> TaskDatum;
};

/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UMySaveGIS : public UGameInstanceSubsystem
{
	
	GENERATED_BODY()
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
public:
	FAllDataToSave Global_AllDataToSave;
	UFUNCTION(BlueprintCallable, Category = "SaveData")
	bool SaveData(const FString& Filename, FAllDataToSave AllDataToSave);

	UFUNCTION(BlueprintCallable, Category = "LoadData")
	bool LoadData(const FString& Filename, FAllDataToSave& AllDataToSave);
};
