// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncNode_IntervalCall.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncActionOperation);

// This class exposes an output pin of its instance when use properties down here
UCLASS(BlueprintType, meta=(ExposedAsyncProxy=AsyncUObjectInstance))
class MYREWARDPROJECT_API UAsyncNode_IntervalCall : public UBlueprintAsyncActionBase
{
public:
	/**
	  * Calls a function at regular intervals and allows the loop to be canceled(Drag the ouput pin instance to call CancelUpdate).
	  * @param WorldContextObject A context object that provides access to the world. 
	  * @param IntervalCall Interval in seconds between function calls. 
	  * @param MaxDuration If greater than zero, the loop ends when TotalTime exceeds EndTime. If set to -1, the loop continues indefinitely.
	  */
	UFUNCTION(BlueprintInternalUseOnly, BlueprintCallable, Category="Jeff|Utilities",
		meta=(WorldContext="WorldContextObject"))
	static UAsyncNode_IntervalCall* CreateAsyncIntervalCall(UObject* WorldContextObject, float IntervalCall = 1.f,
	                                                      float MaxDuration = -1);
	void ResetVariables();

	//Execute Function when Completed
	UPROPERTY(BlueprintAssignable)
	FOnAsyncActionOperation OnCompleted;
	UPROPERTY(BlueprintAssignable)
	FOnAsyncActionOperation OnUpdate;
	UPROPERTY(BlueprintAssignable)
	FOnAsyncActionOperation OnCancel;

	UFUNCTION(BlueprintCallable)
	void CancelUpdate();

protected:
	void UpdateFunction();
	virtual void Activate() override;

	UPROPERTY()
	UWorld* World;
	UPROPERTY()
	UObject* WorldContextObject;

	float IntervalCall;
	float MaxDuration;

protected:
	FTimerHandle TimeHandle;
	float CurrentTotalTime;

	GENERATED_BODY()
};
