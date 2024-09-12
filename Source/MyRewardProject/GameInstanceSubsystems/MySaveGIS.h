// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MySaveGIS.generated.h"

class UScrollBox;

USTRUCT(BlueprintType)

struct FTaskData
{
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	FString SortName;

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	FString Title;
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	FString Detail;

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	float Score;

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 Days; // == 0 -> IsOnce
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 SavedDays;

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 Times;
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 SavedTimes; // == 0 -> IsFinish

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	bool bIsAddScore;

	FTaskData()
	{
		Score = 1;
		Days = 0;
		SavedDays = 0;
		Times = 1;
		SavedTimes = 1;
		bIsAddScore = true;
	}

	bool operator==(const FTaskData& Other) const
	{
		return SortName == Other.SortName &&
			Title == Other.Title &&
			Detail == Other.Detail &&
			FMath::IsNearlyEqual(Score, Other.Score) && // Use IsNearlyEqual for floats
			Days == Other.Days &&
			Times == Other.Times &&
			SavedTimes == Other.SavedTimes;
	}

	GENERATED_BODY()
};

USTRUCT(BlueprintType)

struct FRewardPerDays //todo 模仿原神或者皇室的那个奖励令牌系统，还有每日任务奖励系统，日活要达标
{
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	float DailyBonusPoints;
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	float DailyBonusPointsRewardValue;

	GENERATED_BODY()
};

USTRUCT(BlueprintType)

struct FGlobalData //todo Add to AllDataToSave
{
	UPROPERTY(EditAnywhere, Category=JFSetting)
	float TotalScore;
	GENERATED_BODY()
};


USTRUCT(BlueprintType)

struct FAllDataToSave
{
public:
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	TArray<FTaskData> TaskDatum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float GlobalTotalScore;

	//Daily Reward
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float GlobalDailyProgress = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float GlobalDailyProgress_Saved;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	float DailyProgressRewardValue = 5;
	GENERATED_BODY()
};

/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UMySaveGIS : public UGameInstanceSubsystem
{
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	void AddChildrenToBasicDatum(UScrollBox* InScrollBox);

	UFUNCTION(BlueprintCallable)
	void SaveAllData();
	UFUNCTION(BlueprintCallable)
	void AddScore(float AddNum);
	UFUNCTION(BlueprintCallable)
	void MinusScore(float MinusNum);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetScore();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetGlobalDailyProgress_Saved();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetDailyProgressRewardValue();
	FAllDataToSave Global_AllDataToSave;

	float AnotherDay = 0;

	UFUNCTION(BlueprintCallable, Category = "SaveData")
	bool SaveData(FAllDataToSave AllDataToSave);

	UFUNCTION(BlueprintCallable, Category = "LoadData")
	bool LoadData(FAllDataToSave& AllDataToSave);

	GENERATED_BODY()
};
