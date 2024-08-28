// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_FunctionUtilities.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AActor* UBFL_FunctionUtilities::SpawnActorAtWidgetWorldPosition(UWidget* Widget, TSubclassOf<AActor> ActorToSpawn,
                                                                APlayerController* PlayerController)
{
	if (!Widget || !ActorToSpawn || !PlayerController)
	{
		return nullptr;
	}

	// Get the underlying widget's geometry
	FGeometry CachedGeometry = Widget->GetCachedGeometry();

	// Get the screen position of the widget
	FVector2D ScreenPosition = CachedGeometry.GetAbsolutePosition();

	// Convert the screen position to world position
	FVector WorldLocation, WorldDirection;
	UWorld* World = PlayerController->GetWorld();
	if (PlayerController->DeprojectScreenPositionToWorld(ScreenPosition.X, ScreenPosition.Y, WorldLocation,
	                                                     WorldDirection))
	{
		// Spawn the actor at the world location
		FActorSpawnParameters SpawnParams;
		if (World)
		{
			return World->SpawnActor<AActor>(ActorToSpawn, WorldLocation, FRotator::ZeroRotator,
			                                 SpawnParams);
		}
	}
	return nullptr;
}
