
#include "UploadFileAsyncTask.h"

#include "WebImage.h"

UCLASS()
class VRTESTPROJECT0903_API UUploadFileAsyncTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUploadComplete, bool, bSuccess, const FString&, Message);

    UPROPERTY(BlueprintAssignable)
    FOnUploadComplete OnSuccess;

    UPROPERTY(BlueprintAssignable) 
    FOnUploadComplete OnFailure;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "File Operations"))
    static UUploadFileAsyncTask* UploadFileToURLWithAPI(const FString& URL, const FString& AuthorizationName, 
        const FString& AuthorizationValue, const FString& VerbOrMethod, const FString& ContentTypeName,
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
    void HandleRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};

UUploadFileAsyncTask* UUploadFileAsyncTask::UploadFileToURLWithAPI(const FString& InURL, const FString& InAuthorizationName,
    const FString& InAuthorizationValue, const FString& InVerbOrMethod, const FString& InContentTypeName,
    const FString& InContentTypeValue)
{
    UUploadFileAsyncTask* Task = NewObject<UUploadFileAsyncTask>();
    Task->URL = InURL;
    Task->AuthorizationName = InAuthorizationName;
    Task->AuthorizationValue = InAuthorizationValue;
    Task->VerbOrMethod = InVerbOrMethod;
    Task->ContentTypeName = InContentTypeName;
    Task->ContentTypeValue = InContentTypeValue;
    return Task;
}

void UUploadFileAsyncTask::Activate()
{
    ExecuteUpload();
}

void UUploadFileAsyncTask::ExecuteUpload()
{
    // Validate input parameters
    if (URL.IsEmpty() || AuthorizationName.IsEmpty() || AuthorizationValue.IsEmpty() ||
        VerbOrMethod.IsEmpty() || ContentTypeName.IsEmpty() || ContentTypeValue.IsEmpty())
    {
        OnFailure.Broadcast(false, TEXT("Invalid input parameters"));
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
    if (!Request.IsValid())
    {
        OnFailure.Broadcast(false, TEXT("Failed to create HTTP request"));
        return;
    }

    Request->SetURL(URL);
    Request->SetVerb(VerbOrMethod);
    Request->SetHeader(ContentTypeName, ContentTypeValue);
    Request->SetHeader(AuthorizationName, AuthorizationValue);

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
            Request->OnProcessRequestComplete().BindUObject(this, &UUploadFileAsyncTask::HandleRequestComplete);

            if (!Request->ProcessRequest())
            {
                OnFailure.Broadcast(false, TEXT("Failed to process HTTP request"));
                return;
            }
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

void UUploadFileAsyncTask::HandleRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!Request.IsValid() || !Response.IsValid())
    {
        OnFailure.Broadcast(false, TEXT("Invalid Request or Response"));
        return;
    }

    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        FString SuccessMessage = FString::Printf(TEXT("Success: %s"), *Response->GetContentAsString());
        OnSuccess.Broadcast(true, SuccessMessage);
    }
    else
    {
        FString ErrorMessage = FString::Printf(TEXT("HTTP Request failed: %s"),
            Response.IsValid() ? *Response->GetContentAsString() : TEXT("Invalid Response"));
        OnFailure.Broadcast(false, ErrorMessage);
    }
}
