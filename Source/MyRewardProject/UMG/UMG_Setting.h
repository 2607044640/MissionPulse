// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG_BasicTask.h"
#include "Blueprint/UserWidget.h"
#include "MyRewardProject/GameInstanceSubsystems/MySaveGIS.h"
#include "UMG_Setting.generated.h"

class UUMG_BasicEditer;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_Setting : public UUserWidget
{
	virtual void NativeConstruct() override;
public:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_URL;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_VerbOrMethod_Save;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_VerbOrMethod_Load;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_ContentTypeValue;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_ContentTypeName;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_AuthorizationValue;
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UUMG_BasicEditer* BasicEditer_AuthorizationName;
private:
	UPROPERTY()
	UMySaveGIS* MySaveGIS;
	GENERATED_BODY()
};
