#include "SendHttpRequestWithAPI.h"

#include "HttpModule.h"
#include "WebImage.h"
#include "MyRewardProject/MyRewardProject.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"

/**
 * Sends an HTTP request to server. Requires API key for authorization value and header (AuthorizationName)
  ,along with Content Type Name & Value, if either the Name or Value is empty, the corresponding request header will not be set
 * @param URL The target URL to send request to
 * @param AuthorizationName The Authorization header name required by your server
 * @param AuthorizationValue The Authorization header value/API key required by your server  
 * @param VerbOrMethod HTTP verb to use: PUT/PATCH/GET/DELETE/HEAD/OPTIONS
 * @param ContentTypeName The Content-Type header name required for the request
 * @param ContentTypeValue The Content-Type header value for the request
  * @return Instance of the request task
 */

USendHttpRequestWithAPI* USendHttpRequestWithAPI::SendHttpRequestWithAPI(const FString& InURL,
                                                                         const FString& InAuthorizationName,
                                                                         const FString& InAuthorizationValue,
                                                                         const FString& InVerbOrMethod,
                                                                         const FString& InContentTypeName,
                                                                         const FString& InContentTypeValue)
{
	USendHttpRequestWithAPI* Task = NewObject<USendHttpRequestWithAPI>();

	Task->URL = InURL;
	Task->AuthorizationName = InAuthorizationName;
	Task->AuthorizationValue = InAuthorizationValue;

	FString SanitizedVerb = InVerbOrMethod.TrimStartAndEnd().ToUpper();
	if (SanitizedVerb != TEXT("GET") &&
		SanitizedVerb != TEXT("POST") &&
		SanitizedVerb != TEXT("PUT") &&
		SanitizedVerb != TEXT("DELETE") &&
		SanitizedVerb != TEXT("PATCH") &&
		SanitizedVerb != TEXT("HEAD") &&
		SanitizedVerb != TEXT("OPTIONS"))
	{
		// Default to PUT if invalid verb provided
		Task->VerbOrMethod = TEXT("PUT");
	}
	else
	{
		Task->VerbOrMethod = SanitizedVerb;
	}


	// Task->VerbOrMethod = InVerbOrMethod;
	Task->ContentTypeName = InContentTypeName;
	Task->ContentTypeValue = InContentTypeValue;
	return Task;
}

void USendHttpRequestWithAPI::Activate()
{
	ExecuteUpload();
}

bool USendHttpRequestWithAPI::BindRequestAndProcessBroadcast(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> InRequest)
{
	InRequest->OnProcessRequestComplete().BindLambda(
		[&](FHttpRequestPtr RequestTemp, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			if (!RequestTemp.IsValid() || !Response.IsValid())
			{
				OnFailure.Broadcast(false, TEXT("Invalid Request or Response"));
				return;
			}

			if (bWasSuccessful && Response->GetResponseCode() == 200)
			{
				// We need to preserve the pure JSON string, so do not add anything to the field that corresponding to GET
				FString SuccessMessage = FString::Printf(
					TEXT("%s%s"), VerbOrMethod.Equals(TEXT("GET")) ? TEXT("") : TEXT("Successfully saved : "),
					*Response->GetContentAsString());
				OnSuccess.Broadcast(true, SuccessMessage);
			}
			else
			{
				FString ErrorMessage = FString::Printf(TEXT("HTTP Request failed: %s"),
				                                       Response.IsValid()
					                                       ? *Response->GetContentAsString()
					                                       : TEXT("Invalid Response"));
				OnFailure.Broadcast(false, ErrorMessage);
			}
		});


	if (!InRequest->ProcessRequest())
	{
		OnFailure.Broadcast(false, TEXT("Failed to process HTTP request"));
		return true;
	}
	return false;
}

void USendHttpRequestWithAPI::ExecuteUpload()
{
	// Validate input parameters
	if (URL.IsEmpty() || VerbOrMethod.IsEmpty())
	{
		OnFailure.Broadcast(false, TEXT("Invalid input URL or VerbOrMethod"));
		return;
	}

	// Ensure HTTP module is loaded
	if (!FModuleManager::Get().IsModuleLoaded("HTTP"))
	{
		OnFailure.Broadcast(false, TEXT("HTTP module not loaded"));
		return;
	}

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
	{
		OnFailure.Broadcast(false, TEXT("Failed to get HTTP module"));
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->SetURL(URL);
	Request->SetVerb(VerbOrMethod);

	// Set headers only if necessary
	if (!AuthorizationName.IsEmpty() && !AuthorizationValue.IsEmpty())
	{
		Request->SetHeader(AuthorizationName, AuthorizationValue);
	}

	if (VerbOrMethod == TEXT("GET"))
	{
		if (BindRequestAndProcessBroadcast(Request)) return;
	}
	else
	{
		if (!ContentTypeName.IsEmpty() && !ContentTypeValue.IsEmpty())
		{
			Request->SetHeader(ContentTypeName, ContentTypeValue);
		}

		if (!SaveDataFileName.IsEmpty())
		{
			FString FilePath = FPaths::ProjectDir() + TEXT("Saved/MySavedFolder/") + SaveDataFileName;
			if (!FPaths::FileExists(FilePath))
			{
				OnFailure.Broadcast(false, FString::Printf(TEXT("JSON file does not exist at path: %s"), *FilePath));
				return;
			}

			FString JsonContent;
			if (FFileHelper::LoadFileToString(JsonContent, *FilePath))
			{
				Request->SetContentAsString(JsonContent);

				if (BindRequestAndProcessBroadcast(Request)) return;
			}
			else
			{
				OnFailure.Broadcast(false, TEXT("Failed to load JSON file"));
				return;
			}
		}
		else
		{
			OnFailure.Broadcast(false, TEXT("SaveDataFileName is empty"));
			return;
		}
	}
}
