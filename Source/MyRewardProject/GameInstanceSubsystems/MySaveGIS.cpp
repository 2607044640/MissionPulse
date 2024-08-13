// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGIS.h"

#include "MyRewardProject/MyRewardProject.h"


void UMySaveGIS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadData(SaveDataFileName, Global_AllDataToSave);
}

bool UMySaveGIS::SaveData(const FString& Filename, FAllDataToSave AllDataToSave)
{
	TArray<TSharedPtr<FJsonValue>> JsonValues;

	for (FTaskData
		 TaskData : AllDataToSave.TaskDatum)
	{
		TSharedPtr<FJsonObject> JsonObject(new FJsonObject);

		JsonObject->SetStringField(TEXT("SortName"), TaskData.SortName);

		JsonObject->SetStringField(TEXT("Title"), TaskData.Title);
		JsonObject->SetStringField(TEXT("Detail"), TaskData.Detail);

		JsonObject->SetNumberField(TEXT("Score"), TaskData.Score);

		JsonObject->SetNumberField(TEXT("Days"), TaskData.Days);
		JsonObject->SetNumberField(TEXT("Times"), TaskData.Times);
		
		JsonObject->SetNumberField(TEXT("SavedTimes"), TaskData.SavedTimes);

		JsonValues.Add(MakeShareable(new FJsonValueObject(JsonObject)));
	}
	FString JsonStr;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonStr);

	if (FJsonSerializer::Serialize(JsonValues, JsonWriter))
	{
		FString TempStr = FString::Printf(TEXT("%s"), *JsonStr);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, false);
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		// Save the JSON string to a file
		FString FilePath = FPaths::ProjectDir() + TEXT("Saved/MySavedFolder/") + Filename;
		if (FFileHelper::SaveStringToFile(JsonStr, *FilePath))
		{
			FString TempStr1 = FString::Printf(TEXT("File saved successfully at %s"), *FilePath);
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr1, true);
			UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr1);

			return true;
		}
	}
	return false;
}

bool UMySaveGIS::LoadData(const FString& Filename, FAllDataToSave& AllDataToSave)
{
	FString FilePath = FPaths::ProjectDir() + TEXT("Saved/MySavedFolder/") + Filename;
	FString Result;
	if (FFileHelper::LoadFileToString(Result, *FilePath))
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Result);
		TArray<TSharedPtr<FJsonValue>> JsonValues;

		if (FJsonSerializer::Deserialize(JsonReader, JsonValues))
		{
			for (TSharedPtr<FJsonValue>
			     JsonValue : JsonValues)
			{
				FTaskData TaskData;
				TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();

				TaskData.SortName = *JsonObject->GetStringField(TEXT("SortName"));
				TaskData.Title = *JsonObject->GetStringField(TEXT("Title"));
				TaskData.Detail = *JsonObject->GetStringField(TEXT("Detail"));

				TaskData.Score = JsonObject->GetNumberField(TEXT("Score"));

				TaskData.Days = JsonObject->GetNumberField(TEXT("Days"));
				TaskData.Times = JsonObject->GetNumberField(TEXT("Times"));
				
				TaskData.SavedTimes = JsonObject->GetNumberField(TEXT("SavedTimes"));

				AllDataToSave.TaskDatum.Add(TaskData);
			}
		}
		return true;
	}
	return false;
}
