// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include <winuser.h>

#include "AMF.h"

void UMyGameInstance::Shutdown()
{
	Super::Shutdown(); //Put super shutdown at end
}
void UMyGameInstance::Init()
{
	Super::Init();

	// Bind to focus changing delegate
	FSlateApplication::Get().OnFocusChanging().AddUObject(this, &UMyGameInstance::Test);
}


void UMyGameInstance::Test(const FFocusEvent& FocusEvent, const FWeakWidgetPath& WeakWidgetPath,
	const TSharedPtr<SWidget>& Widget, const FWidgetPath& WidgetPath, const TSharedPtr<SWidget>& Shared)
{

	const void* WindowHandleConst = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
	HWND hwnd = reinterpret_cast<HWND>(const_cast<void*>(WindowHandleConst)); // Cast const void* to HWND

	// Check if the window is active
	if (Widget)
	{
		FSlateApplication::Get().ClearAllUserFocus(EFocusCause::Cleared);

		// If focused, minimize the window
		if (IsIconic(hwnd)) // IsIconic checks if the window is minimized
		{
			ShowWindow(hwnd, SW_RESTORE); // Restore from minimized
		}
		else
		{
			ShowWindow(hwnd, SW_MINIMIZE); // Minimize the window
		}
	}

	if (Shared)
	{
		UE_LOG(LogTemp, Log, TEXT("New focus: %s"), *Shared->ToString());
	}
	
	// if (Shared)
	// {
	// 	TSharedPtr<SWidget> WidgetToFocus; // Initialize this with your widget
	// 	FSlateApplication::Get().SetUserFocus(0, WidgetToFocus, EFocusCause::SetDirectly);
	// 	
	// 	UE_LOG(LogTemp, Log, TEXT("New focus: %s"), *Shared->ToString());
	// }
	//
	// if (Widget)
	// {
	// 	FSlateApplication::Get().ClearAllUserFocus(EFocusCause::Cleared);
	// 	UE_LOG(LogTemp, Log, TEXT("Old focus: %s"), *Widget->ToString());
	// }
}
 