// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_Setting.h"
#include "UMG_BasicEditer.h"
#include "Components/TextBlock.h"

#define AddLambdaToEditer(BasicEditer,DataToOperate)\
BasicEditer->OnEditFinishedCommitted.AddLambda([&](UUMG_BasicTask* Uumg_BasicTask, FText Text)\
{\
	MySaveGIS->Global_AllDataToSave.DataToOperate = Text.ToString();\
});\
BasicEditer->TextBlock->SetText(FText::FromString(MySaveGIS->Global_AllDataToSave.DataToOperate));

void UUMG_Setting::NativeConstruct()
{
	MySaveGIS = GetWorld()->GetGameInstance()->GetSubsystem<UMySaveGIS>();
	AddLambdaToEditer(BasicEditer_URL, URL)
	AddLambdaToEditer(BasicEditer_VerbOrMethod_Save,VerbOrMethod_Save)
	AddLambdaToEditer(BasicEditer_VerbOrMethod_Load,VerbOrMethod_Load)
	AddLambdaToEditer(BasicEditer_AuthorizationName,AuthorizationName)
	AddLambdaToEditer(BasicEditer_AuthorizationValue,AuthorizationValue)
	AddLambdaToEditer(BasicEditer_ContentTypeName,ContentTypeName)
	AddLambdaToEditer(BasicEditer_ContentTypeValue,ContentTypeValue)
	Super::NativeConstruct();
}
