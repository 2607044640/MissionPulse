// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGIS.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "MyRewardProject/UMG/UMG_TasksContainer.h"
#include "Kismet/GameplayStatics.h"

#include "Components/ScrollBox.h"
#include "HttpModule.h"

#if !PLATFORM_ANDROID
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#endif

#include "Interfaces/IHttpResponse.h"
#include "MyRewardProject/MyRewardProject.h"
#include "MyRewardProject/BlueprintFunctionLibraries/BFL_GetClasses.h"
#include "MyRewardProject/Frameworks/MyHUD.h"
#include "MyRewardProject/UMG/UMG_BasicTask.h"
#include "MyRewardProject/UMG/UMG_MainUI.h"

#if !PLATFORM_ANDROID
#include "Interfaces/IHttpRequest.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#endif

// 1. Core System Functions
/**
 * Initializes the subsystem and loads saved data
 * @param Collection The subsystem collection
 */
void UMySaveGIS::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadData();
}

// 2. Data Integration
/**
 * Integrates local and web-based task data
 * @param WebSavedString JSON string containing web data
 * @return if integrate succeed
 */
bool UMySaveGIS::IntegrateLocalAndWebToAllDataToSave(const FString& WebSavedString)
{

	// Cache local data before integration
	FAllDataToSave LocalData = Global_AllDataToSave;

	// Attempt to parse web data
	if (!AnalysisLoadedStringToAllDataToSave(WebSavedString, true))
	{
		// Restore local data if parsing fails
		Global_AllDataToSave = LocalData;

		return false;
	}
	FAllDataToSave WebData = Global_AllDataToSave;


	// Integrate web tasks with local tasks
	int32 ScoreAdjustment = 0;
	for (const FTaskData& WebTask : WebData.TaskDatum)
	{
		bool FoundTask = false;
		// Update existing task or add new one
		for (FTaskData& LocalTask : LocalData.TaskDatum)
		{
			if (LocalTask.SpawnTime == WebTask.SpawnTime)
			{
				FoundTask = true;

				// Compare timestamps to determine latest version
				int32 TempTimeChange = 0; //record the operation of SavedTimes(How user clicked)
				int32 LatestScore = LocalTask.Score;
				if (LocalTask.ClickTime >= WebTask.ClickTime)
				{
					break; //if local task is the one which is latest, then don't change anything
				}
				else
				{
					//before - latest
					TempTimeChange = LocalTask.ChangedSavedTimes - WebTask.ChangedSavedTimes;
					// Use web version if more recent

					int32 TempSavedTimes = LocalTask.SavedTimes;
					int64 LocalDay = LocalTask.ClickTime - (LocalTask.ClickTime % ETimespan::TicksPerDay);
					int64 WebDay = WebTask.ClickTime - (WebTask.ClickTime % ETimespan::TicksPerDay);

					LocalTask = WebTask;
					LatestScore = WebTask.Score;

					if (LocalTask.Days != 0 && LocalDay != WebDay)
					// if the task is the type of daily task && not the same day
					// then use local(because the SavedTimes refresh everyday)
					{
						LocalTask.SavedTimes = TempSavedTimes;
					}
				}
				if (TempTimeChange)
				{
					ScoreAdjustment += TempTimeChange * LatestScore * (WebTask.bIsAddScore ? 1 : -1);
				}
				break;
			}
		}
		if (!FoundTask)
		{
			// Add new task from web data
			LocalData.TaskDatum.Add(WebTask); //todo we may want to adjust the position of task

			//adjust score when new task from web is finished or clicked
			if (WebTask.ChangedSavedTimes)
			{
				/*need reverse ChangedSavedTimes*/
				ScoreAdjustment += (-WebTask.ChangedSavedTimes) * WebTask.Score * (WebTask.bIsAddScore ? 1 : -1);
			}
		}
	}

	//Adjust local Score according ScoreAdjustment
	if (ScoreAdjustment > 0)
	{
		LocalData.GlobalTotalScore += ScoreAdjustment;
		LocalData.GlobalDailyProgress_Saved += ScoreAdjustment;

		if (LocalData.GlobalDailyProgress <= LocalData.GlobalDailyProgress_Saved &&
			LocalData.GlobalDailyProgress)
		{
			// Calculate the quotient (how many times the divisor fits into the dividend)
			int32 Quotient = LocalData.GlobalDailyProgress_Saved / LocalData.GlobalDailyProgress;

			// Calculate the remainder (what's left after division)
			// int32 Remainder = remainder(LocalData.GlobalDailyProgress_Saved,
			// LocalData.GlobalDailyProgress);

			for (int i = 0; i < Quotient; ++i)
			{
				LocalData.GlobalDailyProgress_Saved -= LocalData.GlobalDailyProgress;
				LocalData.GlobalTotalScore += LocalData.DailyProgressRewardValue;
			}
		}
	}
	else
	{
		LocalData.GlobalTotalScore += ScoreAdjustment;
	}

	// Use web devices as default value(the devices data from web will always be correct) 
	LocalData.Devices = WebData.Devices;

	// Persist merged data
	Global_AllDataToSave = LocalData;
	return true;
}


// 3. Task Management
/**
 * Adds task widgets from a scroll box to basic data
 * @param InScrollBox The scroll box containing task widgets
 */
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
			// use EmplaceAt_GetRef function to construct new TaskData directly
			Global_AllDataToSave.TaskDatum.EmplaceAt(
				Global_AllDataToSave.TaskDatum.Num(),
				UMG_BasicTask->TaskData
			);
		}
	}
}

// 4. Device Management
/**
 * Finds or creates a device entry in the devices array
 * @param InDevices Array of devices
 * @param DeviceID Unique device identifier
 * @return Reference to found or new device
 */
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


#if !PLATFORM_ANDROID
// 5. System Identification
/**
 * Retrieves the system drive serial number
 * @return Serial number as string, empty if failed
 */
FString UMySaveGIS::GetSystemDriveSerialNumber()
{
	FString SerialNumber;

#if PLATFORM_WINDOWS
	TCHAR WindowsPath[MAX_PATH];
	GetWindowsDirectory(WindowsPath, MAX_PATH);
	FString DriveLetter = FString(WindowsPath).Left(2); // Get "C:"
	DriveLetter += TEXT("\\");

	TCHAR VolumeNameBuffer[MAX_PATH];
	DWORD VolumeSerialNumber;
	DWORD MaxComponentLength;
	DWORD FileSystemFlags;
	TCHAR FileSystemNameBuffer[MAX_PATH];

	if (GetVolumeInformation(
		*DriveLetter,
		VolumeNameBuffer,
		MAX_PATH,
		&VolumeSerialNumber,
		&MaxComponentLength,
		&FileSystemFlags,
		FileSystemNameBuffer,
		MAX_PATH))
	{
		SerialNumber = FString::Printf(TEXT("%08X"), VolumeSerialNumber);
		return SerialNumber;
	}
#endif

	// 如果获取失败或不是Windows平台，使用备选方案
	return FPlatformMisc::GetDeviceId();
}
#endif

/**
 * Generates a unique device identifier
 * @return Device ID string
 */
FString UMySaveGIS::GenerateDeviceId()
{
	FString DeviceId;
	DeviceId = UKismetSystemLibrary::GetDeviceId();

	if (DeviceId.IsEmpty())
	{
#if !PLATFORM_ANDROID
		DeviceId = GetSystemDriveSerialNumber();
#endif
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

// 6. Data Persistence
/**
 * Saves all current task and system data
 */
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

// 7. Score Management
/**
 * Adds points to global score and daily progress
 * @param AddNum Amount to add
 */
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

/**
 * Subtracts points from global score
 * @param MinusNum Amount to subtract
 */
void UMySaveGIS::MinusScore(float MinusNum)
{
	Global_AllDataToSave.GlobalTotalScore -= MinusNum;
}

// 8. Data Retrieval
/**
 * Gets current global score
 * @return Current score value
 */
float UMySaveGIS::GetScore()
{
	return Global_AllDataToSave.GlobalTotalScore;
}

/**
 * Gets saved daily progress
 * @return Current daily progress value
 */
float UMySaveGIS::GetGlobalDailyProgress_Saved()
{
	return Global_AllDataToSave.GlobalDailyProgress_Saved;
}

float UMySaveGIS::GetDailyProgressRewardValue()
{
	return Global_AllDataToSave.DailyProgressRewardValue;
}

// 9. Task Initialization
/**
 * Initializes tasks from global data with delay
 */
void UMySaveGIS::DelayToInitializeTasksFromGlobalData()
{
	if (AMyHUD* MyHUD = Cast<AMyHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()))
	{
		MyHUD->MainUI->TasksContainer->RegenerateTasksFromGlobalData();
	}
}


// 10. Data Serialization
/**
 * Saves game data to persistent storage
 * @param AllDataToSave Data structure to save
 * @return Success status
 */
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

		TaskObject->SetNumberField(TEXT("ChangedSavedTimes"), TaskData.ChangedSavedTimes);

		TaskObject->SetBoolField(TEXT("bIsAddScore"), TaskData.bIsAddScore);

		TaskObject->SetStringField(TEXT("SpawnTime"), FString::Printf(TEXT("%lld"), TaskData.SpawnTime));
		TaskObject->SetStringField(TEXT("ClickTime"), FString::Printf(TEXT("%lld"), TaskData.ClickTime));

		TaskDatumJsonValues.Add(MakeShareable(new FJsonValueObject(TaskObject)));
	}
	MainJsonObject->SetArrayField(TEXT("TaskData"), TaskDatumJsonValues);

	//Devices

	TArray<TSharedPtr<FJsonValue>> DevicesArray;
	for (const FDevice& Device : AllDataToSave.Devices)
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

	OtherJsonObject->SetNumberField(TEXT("GlobalDayToRecord"), GetDateTimeTodayTicks() / ETimespan::TicksPerDay);
	OtherJsonObject->SetNumberField(TEXT("GlobalTotalScore"), AllDataToSave.GlobalTotalScore);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress"), AllDataToSave.GlobalDailyProgress);
	OtherJsonObject->SetNumberField(TEXT("GlobalDailyProgress_Saved"), AllDataToSave.GlobalDailyProgress_Saved);
	OtherJsonObject->SetNumberField(TEXT("DailyProgressRewardValue"), AllDataToSave.DailyProgressRewardValue);

	OtherJsonObject->SetStringField(TEXT("URL"), AllDataToSave.URL);
	OtherJsonObject->SetStringField(TEXT("AuthorizationName"), AllDataToSave.AuthorizationName);
	OtherJsonObject->SetStringField(TEXT("AuthorizationValue"), AllDataToSave.AuthorizationValue);
	OtherJsonObject->SetStringField(TEXT("ContentTypeName"), AllDataToSave.ContentTypeName);
	OtherJsonObject->SetStringField(TEXT("ContentTypeValue"), AllDataToSave.ContentTypeValue);

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


// 11. Network Operations
/**
 * Fetches and parses JSON data from URL
 * @param Url Source URL for JSON data
 *
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
								TaskObject->TryGetNumberField(TEXT("ChangedSavedTimes"), TaskData.ChangedSavedTimes);
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
*/
/**
 * Analyzes loaded string data and converts to game data
 * @param Result JSON string to analyze
 * @param IsGETRequest Flag indicating if from GET request
 * @return Success status
 */
bool UMySaveGIS::AnalysisLoadedStringToAllDataToSave(FString Result, bool bParseRawData_GetRequest)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Result);
	TSharedPtr<FJsonObject> JsonObject;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		// For GET requests, we need to parse the "record" object first
		/*if (IsGETRequest)
		{
			if (TSharedPtr<FJsonObject> RecordObject = JsonObject->GetObjectField(TEXT("record")))
			{
				JsonObject = RecordObject;
			}
			else
			{
				return false;
			}
		}*/

		if (bParseRawData_GetRequest)
		{
			TArray<FString> CommonFields = {TEXT("record"), TEXT("data"), TEXT("result"), TEXT("payload")};
			bool bFieldFound = false;

			for (const FString& FieldName : CommonFields)
			{
				if (JsonObject->HasField(FieldName))
				{
					JsonObject = JsonObject->GetObjectField(FieldName);
					bFieldFound = true;
					break;
				}
			}

			if (!bFieldFound && !bParseRawData_GetRequest)
			{
				{
					FString
						TempStr = FString::Printf(TEXT("No common fields found and bParseRawData is false"));
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true,
						                                 FVector2D(3, 3));
					UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
				}

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
					TaskData.ChangedSavedTimes = TaskObject->GetNumberField(TEXT("ChangedSavedTimes"));
					TaskData.bIsAddScore = TaskObject->GetBoolField(TEXT("bIsAddScore"));
					TaskData.SpawnTime = FCString::Atoi64(*TaskObject->GetStringField(TEXT("SpawnTime")));
					TaskData.ClickTime = FCString::Atoi64(*TaskObject->GetStringField(TEXT("ClickTime")));

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

// 12. Device Time Management
/**
 * Sets current device time to now
 */
void UMySaveGIS::SetThisDeviceTimeToNow()
{
	// Add Device
	FDevice& Device = FindDeviceOrAddNewDevice(Global_AllDataToSave.Devices, GenerateDeviceId());
	Device.DeviceDateTime = GetDateTimeNowTicks();
	SaveAllData();
}

/**
 * Sets device time to specific value
 * @param InTimeTicks Time value in ticks
 */
void UMySaveGIS::SetThisDeviceTimeToSpecificTime(int64 InTimeTicks)
{
	// Add Device
	FDevice& Device = FindDeviceOrAddNewDevice(Global_AllDataToSave.Devices, GenerateDeviceId());
	Device.DeviceDateTime = InTimeTicks;
	SaveAllData();
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
