// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRewardGIS.h"

void UMyRewardGIS::AddScore(float AddNum)
{
	Score += AddNum;
}

void UMyRewardGIS::MinusScore(float MinusNum)
{
	Score -= MinusNum;
}
