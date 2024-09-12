// Fill out your copyright notice in the Description page of Project Settings.


#include "BFL_FunctionUtilities.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UUserWidget* UBFL_FunctionUtilities::JFSetWidgetPositionByDPI(UObject* WorldContextObject,
                                                              UWidget* WidgetToGetPos,
                                                              TSubclassOf<UUserWidget> WidgetClassToCreate,
                                                              UUserWidget* ExistWidget_Optional)
{
	if (!WorldContextObject || !WidgetToGetPos || (!ExistWidget_Optional && !WidgetClassToCreate))
	{
		FString TempStr = FString::Printf(
			TEXT("CreateWidgetAtScreenPosition : Ensure the WorldContextObject and WidgetClass are valid"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);

		return nullptr;
	}
	UUserWidget* ObjectPtr;
	if (!ExistWidget_Optional)
	{
		ObjectPtr = CreateWidget<UUserWidget>(
			UGameplayStatics::GetPlayerController(WorldContextObject->GetWorld(), 0), WidgetClassToCreate);
		// Add the widget to the viewport
		ObjectPtr->AddToViewport();
	}
	else
	{
		ObjectPtr = ExistWidget_Optional;
	}

	// Get the geometry
	FGeometry WidgetGeometry = WidgetToGetPos->GetCachedGeometry();
	FVector2D LocalCoordinate(0, 0);
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry, LocalCoordinate, PixelPosition, ViewportPosition);


	// Set the widget's position on the screen
	ObjectPtr->SetPositionInViewport(ViewportPosition, false);

	// Return the created widget
	return ObjectPtr;
}

UUserWidget* UBFL_FunctionUtilities::JFLerpWidgetToAnotherWidget(UObject* WorldContextObject,
                                                                 UUserWidget* WidgetToDoLerp,
                                                                 UWidget* StartWidget, UWidget* EndWidget,
                                                                 float LerpValue)
{
	if (!StartWidget || !EndWidget || !WidgetToDoLerp || !WorldContextObject)
	{
		FString TempStr = FString::Printf(TEXT("LerpWidgetToAnotherWidget Not Valid"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		return nullptr;
	}
	LerpValue = FMath::Clamp(LerpValue, 0.f, 1.f);;

	// Get the geometry
	FGeometry WidgetGeometry = StartWidget->GetCachedGeometry();
	FVector2D LocalCoordinate(0, 0);
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry, LocalCoordinate, PixelPosition, ViewportPosition);

	// Get the geometry
	FGeometry WidgetGeometry1 = EndWidget->GetCachedGeometry();
	FVector2D LocalCoordinate1(0, 0);
	FVector2D PixelPosition1;
	FVector2D ViewportPosition1;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry1, LocalCoordinate1, PixelPosition1, ViewportPosition1);

	FVector2D NewPosition = FMath::Lerp(ViewportPosition, ViewportPosition1, LerpValue);

	WidgetToDoLerp->SetPositionInViewport(NewPosition, false);

	return nullptr;
}


void UBFL_FunctionUtilities::SortPanelWidgetsChildren(UPanelWidget* InPanelWidget)
{
	TArray<UWidget*> Children = InPanelWidget->GetAllChildren();

	Children.Sort([InPanelWidget](const UWidget& A, const UWidget& B)
	{
		int32 IndexA = InPanelWidget->GetChildIndex(&A);
		int32 IndexB = InPanelWidget->GetChildIndex(&B);
		return IndexA < IndexB;
	});

	InPanelWidget->ClearChildren();
	for (UWidget* Child : Children)
	{
		InPanelWidget->AddChild(Child);
	}
}

UUserWidget* UBFL_FunctionUtilities::JF2LerpWidgetWithRandomToAnotherWidget(UObject* WorldContextObject,
                                                                            UUserWidget* WidgetToDoLerp,
                                                                            UWidget* StartWidget, UWidget* EndWidget,
                                                                            float LerpValue, FVector2D RandomPosition)
{
	if (!StartWidget || !EndWidget || !WidgetToDoLerp || !WorldContextObject)
	{
		FString TempStr = FString::Printf(TEXT("LerpWidgetToAnotherWidget Not Valid"));
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TempStr, true, FVector2D(3, 3));
		UE_LOG(LogTemp, Error, TEXT("%s"), *TempStr);
		return nullptr;
	}
	LerpValue = FMath::Clamp(LerpValue, 0.f, 1.f);;

	// Get the geometry
	FGeometry WidgetGeometry = StartWidget->GetCachedGeometry();
	FVector2D LocalCoordinate(0, 0);
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry, LocalCoordinate, PixelPosition, ViewportPosition);

	// Get the geometry
	FGeometry WidgetGeometry1 = EndWidget->GetCachedGeometry();
	FVector2D LocalCoordinate1(0, 0);
	FVector2D PixelPosition1;
	FVector2D ViewportPosition1;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry1, LocalCoordinate1, PixelPosition1, ViewportPosition1);

	FVector2D NewPosition = FMath::Lerp(ViewportPosition, RandomPosition + ViewportPosition, LerpValue);
	FVector2D NewPositionFinal = FMath::Lerp(NewPosition, ViewportPosition1, LerpValue);

	WidgetToDoLerp->SetPositionInViewport(NewPositionFinal, false);

	return nullptr;
}

AActor* UBFL_FunctionUtilities::JFSpawnActorAtWidgetWorldPosition(UObject* WorldContextObject, UWidget* Widget,
                                                                  TSubclassOf<AActor> ActorToSpawn,
                                                                  APlayerController* PlayerController)
{
	if (!Widget || !ActorToSpawn || !PlayerController)
	{
		return nullptr;
	}

	// Convert the screen position to world position
	FVector WorldLocation, WorldDirection;

	// Get the geometry
	FGeometry WidgetGeometry = Widget->GetCachedGeometry();
	FVector2D LocalCoordinate(0, 0);
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry, LocalCoordinate, PixelPosition, ViewportPosition);


	//Local Size (Right Down Corner)
	FVector2D RightDownCorner = WidgetGeometry.GetLocalSize();

	if (PlayerController->DeprojectScreenPositionToWorld(PixelPosition.X, PixelPosition.Y, WorldLocation,
	                                                     WorldDirection))
	{
		// Spawn the actor at the world location
		if (WorldContextObject->GetWorld())
		{
			FActorSpawnParameters SpawnParams;
			return WorldContextObject->GetWorld()->SpawnActor<AActor>(ActorToSpawn, WorldLocation,
			                                                          FRotator::ZeroRotator,
			                                                          SpawnParams);
		}
	}
	return nullptr;
}

bool UBFL_FunctionUtilities::CheckMouseIsInsideOfWidget_DragDropEvent(UObject* WorldContextObject, UWidget* Widget,
                                                                      FVector2D InDragDropEventGetScreenSpacePosition)
{
	// Get the geometry
	FVector2D ViewportPosition = JFGetWidgetViewPortPosition(WorldContextObject, Widget);

	//Calc MousePosition(Regardless the screen size, it will always remain the same and correct position)
	FVector2D LocalMousePositionInWidget = InDragDropEventGetScreenSpacePosition - ViewportPosition;

	//Local Size (Right Down Corner)
	FVector2D RightDownCorner = Widget->GetCachedGeometry().GetLocalSize();

	if (LocalMousePositionInWidget.X > 0 && LocalMousePositionInWidget.Y > 0 &&
		LocalMousePositionInWidget.X < RightDownCorner.X && LocalMousePositionInWidget.Y < RightDownCorner.Y)
	{
		return true;
	}


	return false;
}


float UBFL_FunctionUtilities::JFNumberDeOrIncreaseGradually(float Number, float SavedNumber, float Speed,
                                                            float LessThan)
{
	if (FMath::Abs(Number - SavedNumber) < LessThan)
	{
		return SavedNumber;
	}
	else
	{
		return Speed / 100.f * (SavedNumber > Number ? 1 : -1) + Number;
	}
}

FText UBFL_FunctionUtilities::JFFloatToText(float InputFloat)
{
	FNumberFormattingOptions* NumberFormattingOptions = new FNumberFormattingOptions();
	NumberFormattingOptions->SetUseGrouping(false);
	NumberFormattingOptions->MaximumFractionalDigits = 2;
	return FText::AsNumber(InputFloat, NumberFormattingOptions);
}

FVector2D UBFL_FunctionUtilities::JFGetWidgetViewPortPosition(UObject* WorldContextObject, UWidget* Widget)
{
	FGeometry WidgetGeometry = Widget->GetCachedGeometry();
	FVector2D LocalCoordinate(0, 0);
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport
		(WorldContextObject, WidgetGeometry, LocalCoordinate, PixelPosition, ViewportPosition);

	return ViewportPosition;
}
