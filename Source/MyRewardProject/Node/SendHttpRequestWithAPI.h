// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SendHttpRequestWithAPI.generated.h"

class IHttpRequest;
/**
 * 
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy=AsyncUObjectInstance))
class MYREWARDPROJECT_API USendHttpRequestWithAPI : public UBlueprintAsyncActionBase
{

	bool BindRequestAndBroadcast(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> InRequest);

public:
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUploadComplete, bool, bSuccess, const FString&, Message);

	UPROPERTY(BlueprintAssignable)
	FOnUploadComplete OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnUploadComplete OnFailure;
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
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "File Operations"))
	static USendHttpRequestWithAPI* SendHttpRequestWithAPI(const FString& URL, const FString& AuthorizationName,
	                                                    const FString& AuthorizationValue, const FString& VerbOrMethod,
	                                                    const FString& ContentTypeName,
	                                                    const FString& ContentTypeValue);

	virtual void Activate() override;

private:
	FString URL;
	FString AuthorizationName;
	FString AuthorizationValue;
	FString VerbOrMethod;
	FString ContentTypeName;
	FString ContentTypeValue;
	FString OutDebugMessage;

	void ExecuteUpload();
	GENERATED_BODY()
};
