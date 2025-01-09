// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGIS.h"

#include "Components/ScrollBox.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "MyRewardProject/MyRewardProject.h"
#include "MyRewardProject/BlueprintFunctionLibraries/BFL_GetClasses.h"
#include "MyRewardProject/Frameworks/MyHUD.h"
#include "MyRewardProject/UMG/UMG_BasicTask.h"
#include "MyRewardProject/UMG/UMG_MainUI.h"
#include "MyRewardProject/UMG/UMG_TasksContainer.h"
#include "Interfaces/IHttpRequest.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include <Windows.h>
#include "Microsoft/HideMicrosoftPlatformTypes.h"
#include "Misc/ScopeLock.h"

void UMySaveGIS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadData();
}


void UMySaveGIS::AddChildrenToBasicDatum(UScrollBox* InScrollBox)
{
	if (!InScrollBox || !InScrollBox->GetAllChildren().IsValidIndex(0))
	{
		return;
	}

	for (UWidget* Child : InScrollBox->GetAllChildren())
	{
		if (UUMG_BasicTask* UMG_BasicTask = Cast<UUMG_BasicTask>(Child))
		{
			// 使用 EmplaceAt_GetRef 直接构造新的 TaskData
			Global_AllDataToSave.TaskDatum.EmplaceAt(
				Global_AllDataToSave.TaskDatum.Num(),
				UMG_BasicTask->TaskData
			);
		}
	}
}

//todo need to change InDevice.DeviceDateTime to current time when save the data
FDevice& UMySaveGIS::FindDeviceOrAddNewDevice(TArray<FDevice>& InDevices, const FString& DeviceID)
{
	for (FDevice& InDevice : InDevices)
	{
		if (!InDevice.DeviceID.Compare(DeviceID))
		{
			return InDevice;
		}
	}
	return InDevices.EmplaceAt_GetRef(InDevices.Num(), DeviceID, 0);
}

FString UMySaveGIS::GetSystemDriveSerialNumber()
{
	FString SerialNumber;

	TCHAR WindowsPath[MAX_PATH];
	GetWindowsDirectory(WindowsPath, MAX_PATH);
	FString DriveLetter = FString(WindowsPath).Left(2); // Get "C:"(Default Value)
	DriveLetter += TEXT("\\");

	TCHAR VolumeNameBuffer[MAX_PATH];
	DWORD VolumeSerialNumber;
	DWORD MaxComponentLength;
	DWORD FileSystemFlags;
	TCHAR FileSystemNameBuffer[MAX_PATH];

	// Get device information
	if (GetVolumeInformation(
		*DriveLetter, // 必须以 "C:\\" 格式结尾
		VolumeNameBuffer,
		MAX_PATH,
		&VolumeSerialNumber,
		&MaxComponentLength,
		&FileSystemFlags,
		FileSystemNameBuffer,
		MAX_PATH))
	{
		SerialNumber = FString::Printf(TEXT("%08X"), VolumeSerialNumber);

		/*
		// Add log
		{
			FString TempStr = FString::Printf(TEXT("Successfully got drive serial: %s"), *SerialNumber);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TempStr, true, FVector2D(3, 3));
			}
			UE_LOG(LogTemp, Warning, TEXT("%s"), *TempStr);
		}
		*/
		return SerialNumber;
	}
	/*
	//Add error log
	DWORD Error = GetLastError();
	FString ErrorMsg = FString::Printf(TEXT("Failed to get drive info. Error code: %d"), Error);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, ErrorMsg, true, FVector2D(3, 3));
	}
	UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
	*/
	return TEXT("");
}

FString UMySaveGIS::GenerateDeviceId()
{
	FString DeviceId;
	DeviceId = UKismetSystemLibrary::GetDeviceId();

	if (DeviceId.IsEmpty())
	{
		DeviceId = GetSystemDriveSerialNumber();
	}
	else if (DeviceId.IsEmpty())
	{
		FString ComputerName = FPlatformProcess::ComputerName();
		FString Windows = TEXT("Windows");
		/*
				{
					FString TempStr = FString::Printf(TEXT("%s"), *(Windows + ComputerName + Windows));
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
					}
					UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
				}
		*/
		DeviceId = FMD5::HashAnsiString(*(Windows + ComputerName + Windows));
	}
	return DeviceId;
}


void UMySaveGIS::SaveAllData()
{
	// Save To AllDataToSave	
	UUMG_TasksContainer* TasksContainer = UBFL_GetClasses::GetMainUI(this)->TasksContainer;
	Global_AllDataToSave.TaskDatum.Empty();
	AddChildrenToBasicDatum(TasksContainer->ScrollBox_Tasks);
	AddChildrenToBasicDatum(TasksContainer->ScrollBox_Tasks_Finish);

	//Parse,Serialize
	SaveData(Global_AllDataToSave);
}

void UMySaveGIS::AddScore(float AddNum)
{
	Global_AllDataToSave.GlobalTotalScore += AddNum;
	Global_AllDataToSave.GlobalDailyProgress_Saved += AddNum;

	if (Global_AllDataToSave.GlobalDailyProgress <= Global_AllDataToSave.GlobalDailyProgress_Saved &&
		Global_AllDataToSave.GlobalDailyProgress)
	{
		// Calculate the quotient (how many times the divisor fits into the dividend)
		int32 Quotient = Global_AllDataToSave.GlobalDailyProgress_Saved / Global_AllDataToSave.GlobalDailyProgress;

		// Calculate the remainder (what's left after division)
		// int32 Remainder = remainder(Global_AllDataToSave.GlobalDailyProgress_Saved,
		// Global_AllDataToSave.GlobalDailyProgress);

		for (int i = 0; i < Quotient; ++i)
		{
			Global_AllDataToSave.GlobalDailyProgress_Saved -= Global_AllDataToSave.GlobalDailyProgress;
			Global_AllDataToSave.GlobalTotalScore += Global_AllDataToSave.DailyProgressRewardValue;

			//Effect
			AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD());
			MyHUD->MainUI->TasksContainer->OnFinishDailyProgress();
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


void UMySaveGIS::DelayToInitializeTasksFromGlobalData()
{
	if (AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()))
	{
		MyHUD->MainUI->TasksContainer->RegenerateTasksFromGlobalData();
	}
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
		TaskObject->SetNumberField(TEXT("Score"), (int32)TaskData.Score);
		TaskObject->SetNumberField(TEXT("Days"), TaskData.Days);
		TaskObject->SetNumberField(TEXT("SavedDays"), TaskData.SavedDays);
		TaskObject->SetNumberField(TEXT("Times"), TaskData.Times);
		TaskObject->SetNumberField(TEXT("SavedTimes"), TaskData.SavedTimes);

		TaskObject->SetBoolField(TEXT("bIsAddScore"), TaskData.bIsAddScore);

		TaskObject->SetStringField(TEXT("SpawnTime"), FString::Printf(TEXT("%lld"), TaskData.SpawnTime));

		TaskDatumJsonValues.Add(MakeShareable(new FJsonValueObject(TaskObject)));
	}
	MainJsonObject->SetArrayField(TEXT("TaskData"), TaskDatumJsonValues);

	//Devices

	TArray<TSharedPtr<FJsonValue>> DevicesArray;
	for (const FDevice& Device : Global_AllDataToSave.Devices)
	{
		TSharedPtr<FJsonObject> DeviceObj = MakeShareable(new FJsonObject);
		DeviceObj->SetStringField(TEXT("DeviceID"), Device.DeviceID);
		DeviceObj->SetStringField(TEXT("DeviceDateTime"), FString::Printf(TEXT("%lld"), Device.DeviceDateTime));
		DevicesArray.Add(MakeShareable(new FJsonValueObject(DeviceObj)));
	}
	MainJsonObject->SetArrayField(TEXT("Devices"), DevicesArray);

	//OtherData
	TArray<TSharedPtr<FJsonValue>> OtherJsonValues;
	TSharedPtr<FJsonObject> OtherJsonObject(new FJsonObject);

	OtherJsonObject->SetNumberField(
		TEXT("GlobalDayToRecord"), GetDateTimeTodayTicks() / ETimespan::TicksPerDay);
	OtherJsonObject->SetNumberField(TEXT("GlobalTotalScore"), Global_AllDataToSave.GlobalTotalScore);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress"), Global_AllDataToSave.GlobalDailyProgress);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress_Saved"), Global_AllDataToSave.GlobalDailyProgress_Saved);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress"), Global_AllDataToSave.GlobalDailyProgress);
	OtherJsonObject->SetNumberField(TEXT("DailyProgressRewardValue"), Global_AllDataToSave.DailyProgressRewardValue);

	OtherJsonObject->SetStringField(TEXT("URL"), Global_AllDataToSave.URL);
	OtherJsonObject->SetStringField(TEXT("AuthorizationName"), Global_AllDataToSave.AuthorizationName);
	OtherJsonObject->SetStringField(TEXT("AuthorizationValue"), Global_AllDataToSave.AuthorizationValue);
	OtherJsonObject->SetStringField(TEXT("ContentTypeName"), Global_AllDataToSave.ContentTypeName);
	OtherJsonObject->SetStringField(TEXT("ContentTypeValue"), Global_AllDataToSave.ContentTypeValue);

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


// size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
// {
// 	userp->append((char*)contents, size * nmemb);
// 	return size * nmemb;
// }
//
// void FetchAndParseJSON(const std::string& url)
// {
// 	CURL* curl = curl_easy_init();
// 	if (curl)
// 	{
// 		std::string readBuffer;
// 		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
// 		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
// 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
// 		curl_easy_perform(curl);
// 		curl_easy_cleanup(curl);
// 		/*
// 				// Parse JSON
// 				auto jsonData = parse(readBuffer);
// 		
// 				// Example: Access data
// 				for (const auto& task : jsonData["TaskData"])
// 				{
// 					std::string title = task["Title"];
// 					int score = task["Score"];
// 					// Do something with the data...
// 				}
// 				*/
// 	}
// }


void UMySaveGIS::FetchAndParseJSON(const FString& Url)
{
	// Create the HTTP request
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			// Log the validity of the response
			FString gweg = FString::Printf(
				TEXT("Response.IsValid(): %s"), Response.IsValid() ? TEXT("True") : TEXT("False"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, gweg, true, FVector2D(3, 3));
			UE_LOG(LogTemp, Error, TEXT("%s"), *gweg);

			// Log success of the request
			FString oaigw = FString::Printf(TEXT("bWasSuccessful: %s"), bWasSuccessful ? TEXT("True") : TEXT("False"));
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, oaigw, true, FVector2D(3, 3));
			UE_LOG(LogTemp, Error, TEXT("%s"), *oaigw);

			if (bWasSuccessful && Response.IsValid())
			{
				// Get and trim the raw response
				FString RawResponse = Request.Get()->GetResponse()->GetContentAsString();
				UE_LOG(LogTemp, Error, TEXT("Raw Response: %s"), *RawResponse); // Log the raw response

				// Parse JSON
				TSharedRef<TJsonReader<>> SharedRefJson = TJsonReaderFactory<>::Create(RawResponse);
				TSharedPtr<FJsonObject> OutObjectJson;

				if (FJsonSerializer::Deserialize(SharedRefJson, OutObjectJson))
				{
					// Successfully deserialized
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("JSON Parsed Successfully"), true,
					                                 FVector2D(3, 3));

					const TArray<TSharedPtr<FJsonValue>>* JsonValues;
					if (OutObjectJson->TryGetArrayField(TEXT("TaskData"), JsonValues))
					{
						for (const TSharedPtr<FJsonValue>& JsonValue : *JsonValues)
						{
							if (TSharedPtr<FJsonObject> TaskObject = JsonValue->AsObject())
							{
								FTaskData TaskData;

								// Use TryGet functions to avoid failures
								TaskObject->TryGetStringField(TEXT("SortName"), TaskData.SortName);
								TaskObject->TryGetStringField(TEXT("Title"), TaskData.Title);
								TaskObject->TryGetStringField(TEXT("Detail"), TaskData.Detail);
								TaskObject->TryGetNumberField(TEXT("Score"), TaskData.Score);
								TaskObject->TryGetNumberField(TEXT("Days"), TaskData.Days);
								TaskObject->TryGetNumberField(TEXT("SavedDays"), TaskData.SavedDays);
								TaskObject->TryGetNumberField(TEXT("Times"), TaskData.Times);
								TaskObject->TryGetNumberField(TEXT("SavedTimes"), TaskData.SavedTimes);
								TaskObject->TryGetBoolField(TEXT("bIsAddScore"), TaskData.bIsAddScore);

								Global_AllDataToSave.TaskDatum.Add(TaskData);
							}
						}
					}

					FTimerHandle TempHandle;
					GetWorld()->GetTimerManager().SetTimer(TempHandle, this,
					                                       &UMySaveGIS::DelayToInitializeTasksFromGlobalData, 0.5f);
				}
				else
				{
					// Log error in deserialization
					UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON"));
				}
			}
		});

	// Set up the request
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}


bool UMySaveGIS::AnalysisLoadedStringToAllDataToSave(FString Result, bool IsGETRequest)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Result);
	TSharedPtr<FJsonObject> JsonObject;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		// For GET requests, we need to parse the "record" object first
		if (IsGETRequest)
		{
			if (TSharedPtr<FJsonObject> RecordObject = JsonObject->GetObjectField(TEXT("record")))
			{
				JsonObject = RecordObject;
			}
			else
			{
				return false;
			}
		}

		// Load task data
		const TArray<TSharedPtr<FJsonValue>>* JsonValues;
		if (JsonObject->TryGetArrayField(TEXT("TaskData"), JsonValues))
		{
			// Clear existing task data before loading new ones
			Global_AllDataToSave.TaskDatum.Empty();

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

					TaskData.SpawnTime = FCString::Atoi64(*TaskObject->GetStringField(TEXT("SpawnTime")));

					Global_AllDataToSave.TaskDatum.Add(TaskData);
				}
			}
		}


		// Deserialize Devices 
		const TArray<TSharedPtr<FJsonValue>>* DevicesArray;
		if (JsonObject->TryGetArrayField(TEXT("Devices"), DevicesArray))
		{
			Global_AllDataToSave.Devices.Empty();
			for (const TSharedPtr<FJsonValue>& Value : *DevicesArray)
			{
				if (TSharedPtr<FJsonObject> DeviceObj = Value->AsObject())
				{
					FDevice Device;
					Device.DeviceID = DeviceObj->GetStringField(TEXT("DeviceID"));
					Device.DeviceDateTime = FCString::Atoi64(*DeviceObj->GetStringField(TEXT("DeviceDateTime")));
					Global_AllDataToSave.Devices.Add(Device);
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
					Global_AllDataToSave.GlobalDailyProgress = OtherObject->GetNumberField(TEXT("GlobalDailyProgress"));
					Global_AllDataToSave.GlobalDailyProgress_Saved = OtherObject->GetNumberField(
						TEXT("GlobalDailyProgress_Saved"));

					Global_AllDataToSave.URL = OtherObject->GetStringField(TEXT("URL"));
					Global_AllDataToSave.AuthorizationName = OtherObject->GetStringField(TEXT("AuthorizationName"));
					Global_AllDataToSave.AuthorizationValue = OtherObject->GetStringField(TEXT("AuthorizationValue"));
					Global_AllDataToSave.ContentTypeName = OtherObject->GetStringField(TEXT("ContentTypeName"));
					Global_AllDataToSave.ContentTypeValue = OtherObject->GetStringField(TEXT("ContentTypeValue"));

					// AnotherDay calculation
					int32 TempDayPrevious = OtherObject->GetNumberField(TEXT("GlobalDayToRecord"));
					int64 TempDayNow = GetDateTimeTodayTicks() / ETimespan::TicksPerDay;

					if (TempDayPrevious != TempDayNow && TempDayNow - TempDayPrevious > 0)
					{
						AnotherDay = TempDayNow - TempDayPrevious;
					}
				}
			}
		}

		return true;
	}

	return false;
}

bool UMySaveGIS::LoadData()
{
	FString FilePath = FPaths::ProjectDir() + TEXT("Saved/MySavedFolder/") + SaveDataFileName;
	FString Result;
	if (FFileHelper::LoadFileToString(Result, *FilePath))
	{
		return AnalysisLoadedStringToAllDataToSave(Result);
	}
	return false;
}

void UMySaveGIS::SetThisDeviceTimeToNow()
{
	// Add Device
	FDevice& Device = FindDeviceOrAddNewDevice(Global_AllDataToSave.Devices, GenerateDeviceId());
	Device.DeviceDateTime = GetDateTimeNowTicks();
}

void UMySaveGIS::SetThisDeviceTimeToSpecificTime(int64 InTimeTicks)
{
	// Add Device
	FDevice& Device = FindDeviceOrAddNewDevice(Global_AllDataToSave.Devices, GenerateDeviceId());
	Device.DeviceDateTime = InTimeTicks;
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
