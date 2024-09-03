// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGIS.h"

#include "Components/ScrollBox.h"
#include "MyRewardProject/MyRewardProject.h"
#include "MyRewardProject/BlueprintFunctionLibraries/BFL_GetClasses.h"
#include "MyRewardProject/UMG/UMG_BasicTask.h"
#include "MyRewardProject/UMG/UMG_MainUI.h"
#include "MyRewardProject/UMG/UMG_TasksContainer.h"


void UMySaveGIS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadData(Global_AllDataToSave);
}

void UMySaveGIS::AddChildrenToBasicDatum(UScrollBox* ScrollBox)
{
	if (!ScrollBox->GetAllChildren().IsValidIndex(0))
	{
		return;
	}
	for (UWidget*
	     Child : ScrollBox->GetAllChildren())
	{
		UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(Child);
		Global_AllDataToSave.TaskDatum.Add(UMG_BasicTask->TaskData);
	}
}

void UMySaveGIS::SaveAllData()
{
	// Save To AllDataToSave	
	UUMG_TasksContainer* TasksContainer = UBFL_GetClasses::GetMainUI(this)->TasksContainer;
	Global_AllDataToSave.TaskDatum.Empty();
	AddChildrenToBasicDatum(TasksContainer->ScrollBox_Tasks);
	AddChildrenToBasicDatum(TasksContainer->ScrollBox_Tasks_Finish);
	SaveData(Global_AllDataToSave);
}

void UMySaveGIS::AddScore(float AddNum)
{
	Global_AllDataToSave.GlobalTotalScore += AddNum;
	Global_AllDataToSave.GlobalDailyProgress_Saved += AddNum;

	if (Global_AllDataToSave.GlobalDailyProgress <= Global_AllDataToSave.GlobalDailyProgress_Saved)
	{
		// Convert the values to integers if necessary (assuming they are floats and you want integer division)
		const int32 SavedProgress = static_cast<int32>(Global_AllDataToSave.GlobalDailyProgress_Saved);
		const int32 TotalProgress = static_cast<int32>(Global_AllDataToSave.GlobalDailyProgress);

		// Calculate the quotient (how many times the divisor fits into the dividend)
		int32 Quotient = Global_AllDataToSave.GlobalDailyProgress_Saved / Global_AllDataToSave.GlobalDailyProgress;

		// Calculate the remainder (what's left after division)
		// int32 Remainder = remainder(Global_AllDataToSave.GlobalDailyProgress_Saved,
		                            // Global_AllDataToSave.GlobalDailyProgress);

		for (int i = 0; i < Quotient; ++i)
		{
			Global_AllDataToSave.GlobalDailyProgress_Saved -= Global_AllDataToSave.GlobalDailyProgress;
			Global_AllDataToSave.GlobalTotalScore += Global_AllDataToSave.DailyProgressRewardValue;
		}
	}
}

void UMySaveGIS::MinusScore(float MinusNum)
{
	Global_AllDataToSave.GlobalTotalScore -= MinusNum;
}

float UMySaveGIS::GetScore()
{
	return Global_AllDataToSave.GlobalTotalScore;
}

float UMySaveGIS::GetGlobalDailyProgress_Saved()
{
	return Global_AllDataToSave.GlobalDailyProgress_Saved;
}

float UMySaveGIS::GetDailyProgressRewardValue()
{
	return Global_AllDataToSave.DailyProgressRewardValue;
}

bool UMySaveGIS::SaveData(FAllDataToSave AllDataToSave)
{
	TSharedPtr<FJsonObject> MainJsonObject(new FJsonObject);

	//TaskData
	TArray<TSharedPtr<FJsonValue>> TaskDatumJsonValues;
	for (const FTaskData& TaskData : AllDataToSave.TaskDatum)
	{
		TSharedPtr<FJsonObject> TaskObject(new FJsonObject);

		TaskObject->SetStringField(TEXT("SortName"), TaskData.SortName);
		TaskObject->SetStringField(TEXT("Title"), TaskData.Title);
		TaskObject->SetStringField(TEXT("Detail"), TaskData.Detail);
		TaskObject->SetNumberField(TEXT("Score"), TaskData.Score);
		TaskObject->SetNumberField(TEXT("Days"), TaskData.Days);
		TaskObject->SetNumberField(TEXT("SavedDays"), TaskData.SavedDays);
		TaskObject->SetNumberField(TEXT("Times"), TaskData.Times);
		TaskObject->SetNumberField(TEXT("SavedTimes"), TaskData.SavedTimes);

		TaskObject->SetBoolField(TEXT("bIsAddScore"), TaskData.bIsAddScore);

		TaskDatumJsonValues.Add(MakeShareable(new FJsonValueObject(TaskObject)));
	}
	MainJsonObject->SetArrayField(TEXT("TaskData"), TaskDatumJsonValues);

	//OtherData
	TArray<TSharedPtr<FJsonValue>> OtherJsonValues;
	TSharedPtr<FJsonObject> OtherJsonObject(new FJsonObject);

	OtherJsonObject->SetNumberField(TEXT("GlobalDayToRecord"), FDateTime::Now().GetDay());
	OtherJsonObject->SetNumberField(TEXT("GlobalTotalScore"), Global_AllDataToSave.GlobalTotalScore);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress"), Global_AllDataToSave.GlobalDailyProgress);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress_Saved"), Global_AllDataToSave.GlobalDailyProgress_Saved);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress"), Global_AllDataToSave.GlobalDailyProgress);
	OtherJsonObject->SetNumberField(TEXT("DailyProgressRewardValue"), Global_AllDataToSave.DailyProgressRewardValue);


	OtherJsonValues.Add(MakeShareable(new FJsonValueObject(OtherJsonObject)));

	MainJsonObject->SetArrayField(TEXT("OtherData"), OtherJsonValues);

	// Serialize JSON object to a string
	FString JsonStr;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);

	if (FJsonSerializer::Serialize(MainJsonObject.ToSharedRef(), JsonWriter))
	{
		// Save the JSON string to a file
		FString FilePath = FPaths::ProjectDir() + TEXT("Saved/MySavedFolder/") + SaveDataFileName;
		if (FFileHelper::SaveStringToFile(JsonStr, *FilePath))
		{
			// FString TempStr1 = FString::Printf(TEXT("File saved successfully at %s"), *FilePath);
			// if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr1, true);
			// UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr1);
			return true;
		}
	}
	return false;
}

bool UMySaveGIS::LoadData(FAllDataToSave& AllDataToSave)
{
	FString FilePath = FPaths::ProjectDir() + TEXT("Saved/MySavedFolder/") + SaveDataFileName;
	FString Result;
	if (FFileHelper::LoadFileToString(Result, *FilePath))
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Result);
		TSharedPtr<FJsonObject> JsonObject;

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		{
			// Load task data
			const TArray<TSharedPtr<FJsonValue>>* JsonValues;
			if (JsonObject->TryGetArrayField(TEXT("TaskData"), JsonValues))
			{
				for (const TSharedPtr<FJsonValue>& JsonValue : *JsonValues)
				{
					if (TSharedPtr<FJsonObject> TaskObject = JsonValue->AsObject())
					{
						FTaskData TaskData;
						TaskData.SortName = TaskObject->GetStringField(TEXT("SortName"));
						TaskData.Title = TaskObject->GetStringField(TEXT("Title"));
						TaskData.Detail = TaskObject->GetStringField(TEXT("Detail"));
						TaskData.Score = TaskObject->GetNumberField(TEXT("Score"));
						TaskData.Days = TaskObject->GetNumberField(TEXT("Days"));
						TaskData.SavedDays = TaskObject->GetNumberField(TEXT("SavedDays"));
						TaskData.Times = TaskObject->GetNumberField(TEXT("Times"));
						TaskData.SavedTimes = TaskObject->GetNumberField(TEXT("SavedTimes"));

						TaskData.bIsAddScore = TaskObject->GetBoolField(TEXT("bIsAddScore"));

						AllDataToSave.TaskDatum.Add(TaskData);
					}
				}
			}
			if (JsonObject->TryGetArrayField(TEXT("OtherData"), JsonValues))
			{
				for (const TSharedPtr<FJsonValue>& JsonValue : *JsonValues)
				{
					if (TSharedPtr<FJsonObject> OtherObject = JsonValue->AsObject())
					{
						Global_AllDataToSave.GlobalTotalScore = OtherObject->GetNumberField(TEXT("GlobalTotalScore"));
						Global_AllDataToSave.DailyProgressRewardValue = OtherObject->GetNumberField(
							TEXT("DailyProgressRewardValue"));
						Global_AllDataToSave.GlobalDailyProgress = OtherObject->GetNumberField(
							TEXT("GlobalDailyProgress"));
						Global_AllDataToSave.GlobalDailyProgress_Saved = OtherObject->GetNumberField(
							TEXT("GlobalDailyProgress_Saved"));

						//AnotherDay calc
						int32 TempDayPrevious = OtherObject->GetNumberField(TEXT("GlobalDayToRecord"));
						int32 TempDayNow = FDateTime::Now().GetDay();
						if (TempDayPrevious != TempDayNow && TempDayNow - TempDayPrevious > 0)
						{
							AnotherDay = TempDayNow - TempDayPrevious;
						}
					}
				}
			}

			return true;
		}
	}
	return false;
}

/*
{
	"TaskData": [
		{
			"SortName": "Null",
			"Title": "oijjjj",
			"Detail": "",
			"Score": 1,
			"Days": 0,
			"SavedDays": 0,
			"Times": 1,
			"SavedTimes": 1
		},
		{
			"SortName": "Null",
			"Title": "aaaaaa",
			"Detail": "",
			"Score": 1,
			"Days": 0,
			"SavedDays": 0,
			"Times": 1,
			"SavedTimes": 0
		},
		{
			"SortName": "Null",
			"Title": "333ffff32fffffffffffffof",
			"Detail": "",
			"Score": 1,
			"Days": 2,
			"SavedDays": 0,
			"Times": 1,
			"SavedTimes": 0
		}
	],
	"OtherData": [
		{
			"GlobalTotalScore": 146,
			"GlobalDailyProgress": 0,
			"GlobalDayToRecord": 21
		}
	]
}
*/
