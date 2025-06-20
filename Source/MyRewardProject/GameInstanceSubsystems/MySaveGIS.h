// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/Widget.h"
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
	int32 Days; // == 0 -> IsOnce
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 SavedDays;

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 Times;
	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	int32 SavedTimes; // == 0 -> IsFinish


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	int32 ChangedSavedTimes; //record the operation of SavedTimes(How user clicked)

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	float Score;

	UPROPERTY(EditAnywhere, Category=JFSetting, BlueprintReadWrite)
	bool bIsAddScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	int64 SpawnTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	int64 ClickTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	int64 EditTime;

	FTaskData()
	{
		//This won't be the code to init...
		//Search this one to init the param you want :True init code
		Score = 10;
		Days = 0;
		SavedDays = 0;
		Times = 1;
		SavedTimes = 1;
		bIsAddScore = true;
		SpawnTime = 0;
		ClickTime = 0;
		EditTime = 0;
		ChangedSavedTimes = 0;
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
struct FDevice
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FString DeviceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	int64 DeviceDateTime;

	FDevice() = default;

	FDevice(const FString& InDeviceID, int64 DeviceDateTime)
		: DeviceID(InDeviceID),
		  DeviceDateTime(DeviceDateTime)
	{
	}
};

USTRUCT(BlueprintType)
struct FAllDataToSave
{
	UPROPERTY(EditAnywhere, Category=Basic, BlueprintReadWrite)
	TArray<FTaskData> TaskDatum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Basic)
	float GlobalTotalScore;

	//Daily Reward
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DailyReward)
	float GlobalDailyProgress = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DailyReward)
	float GlobalDailyProgress_Saved;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DailyReward)
	float DailyProgressRewardValue = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FString URL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FString AuthorizationName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FString AuthorizationValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FString ContentTypeName = TEXT("Content-Type");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JFSetting)
	FString ContentTypeValue = TEXT("application/json");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = JFSetting)
	TArray<FDevice> Devices;

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
	UFUNCTION(BlueprintCallable)
	bool IntegrateLocalAndWebToAllDataToSave(UPARAM(ref) const FString& WebSavedString);
	void AddChildrenToBasicDatum(TArray<UWidget*> InChildren);

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


	UPROPERTY()
	FAllDataToSave Global_AllDataToSave;
	UPROPERTY(BlueprintReadOnly)
	float ScoreChangedBefore;
	UPROPERTY(BlueprintReadOnly)
	float PunishMultiplier = 1.2f;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FAllDataToSave& GetGlobal_AllDataToSave() { return Global_AllDataToSave; }

	float AnotherDay = 0;

	UFUNCTION(BlueprintCallable, Category = "SaveData")
	bool SaveData(FAllDataToSave AllDataToSave);

	UFUNCTION(BlueprintCallable)
	void FetchAndParseJSON(const FString& Url);

	UFUNCTION(BlueprintCallable)
	bool AnalysisLoadedStringToAllDataToSave(FString Result, bool bParseRawData_GetRequest = false);
	UFUNCTION(BlueprintCallable, Category = "LoadData")
	bool LoadData();


	//for auto save and auto load
	UFUNCTION(BlueprintCallable)
	FDevice& FindDeviceOrAddNewDevice(UPARAM(ref) TArray<FDevice>& InDevices, const FString& DeviceID);
	UFUNCTION(BlueprintCallable)
	void SetThisDeviceTimeToNow();
	UFUNCTION(BlueprintCallable)
	void SetThisDeviceTimeToSpecificTime(int64 InTimeTicks);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GenerateDeviceId();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FDevice& FindThenGetThisDevice()
	{
		return FindDeviceOrAddNewDevice(Global_AllDataToSave.Devices, GenerateDeviceId());
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int64 GetDateTimeNowTicks()
	{
		return FDateTime::Now().GetTicks();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int64 GetDateTimeTodayTicks()
	{
		return FDateTime::Now().GetDate().GetTicks();
	}

private:
#if !PLATFORM_ANDROID
	FString GetSystemDriveSerialNumber();
#endif

	GENERATED_BODY()
};
