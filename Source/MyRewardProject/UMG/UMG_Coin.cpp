// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG_Coin.h"

#include "MyRewardProject/BlueprintFunctionLibraries/BFL_FunctionUtilities.h"

void UUMG_Coin::OnTimelineTick(float DeltaTime)
{
	// UBFL_FunctionUtilities::JF2LerpWidgetWithRandomToAnotherWidget(this,Image_Coin,)

	// FVector TempEnemyVector = FVector(MyBasicCharacter->GetActorForwardVector() * 3000.f);
	// FVector RandomVector = FVector(TempEnemyVector.X, TempEnemyVector.Y, FMath::RandRange(RandomMin, RandomMax));
	// FVector NewLocation = FMath::Lerp(Image_Coin., MyBasicCharacter->GetActorLocation() + RandomVector,
	                                                // CurveFloat->GetFloatValue(DeltaTime));
	// FVector NewLocationFinal = UKismetMathLibrary::VLerp(NewLocation, MyBasicCharacter->GetActorLocation(),
	                                                     // CurveFloat->GetFloatValue(DeltaTime));
	// SetActorLocation(NewLocationFinal);
}

void UUMG_Coin::OnTimelineEndEvent()
{
	
}

void UUMG_Coin::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	MyTimeline.TickTimeline(InDeltaTime);
}

void UUMG_Coin::NativeConstruct()
{
	Super::NativeConstruct();
	if (CurveFloat)
	{
		FOnTimelineFloat OnTimelineFloatEvent;
		FOnTimelineEvent OnTimelineFinishedEvent;

		OnTimelineFloatEvent.BindUFunction(this, TEXT("OnTimelineTick"));
		OnTimelineFinishedEvent.BindUFunction(this, TEXT("OnTimelineEndEvent"));
		MyTimeline.AddInterpFloat(CurveFloat, OnTimelineFloatEvent);
		MyTimeline.SetTimelineLength(ETimelineLengthMode::TL_LastKeyFrame);

		MyTimeline.SetTimelineFinishedFunc(OnTimelineFinishedEvent);
	}
}
