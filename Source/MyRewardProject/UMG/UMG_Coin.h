// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "UMG_Coin.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class MYREWARDPROJECT_API UUMG_Coin : public UUserWidget
{
public:
	//Timeline
	FTimeline MyTimeline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* CurveFloat;
	UFUNCTION()
	void OnTimelineTick(float DeltaTime);
	UFUNCTION()
	void OnTimelineEndEvent();
	//Timeline
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UImage* Image_Coin;
	GENERATED_BODY()
};
