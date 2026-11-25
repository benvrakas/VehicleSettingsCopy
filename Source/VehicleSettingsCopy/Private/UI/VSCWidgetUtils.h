#pragma once

#include "CoreMinimal.h"

class UButton;
class UUserWidget;
class UWidget;
class UWidgetTree;

// Helpers for building UI from the game's own Blueprint widgets, which C++ can only reach through reflection.
namespace VSCWidgetUtils
{
	// Class path of the vanilla standard button.
	extern const TCHAR* StandardButtonClass;

	// Class path of the vanilla vehicle route tab, which the panel is added to.
	extern const TCHAR* PathManagerClass;

	// Creates a vanilla standard button in Tree, or nullptr (logged) if the class is missing.
	UUserWidget* CreateStandardButton(UWidgetTree* Tree, const FText& Text);

	// Calls the Blueprint function SetText(Text) on Widget; returns false if it has none.
	bool SetWidgetText(UUserWidget* Widget, const FText& Text);

	// Returns the UButton inside a vanilla standard button (its mButton), or nullptr.
	UButton* GetInnerButton(UUserWidget* StandardButton);

	// Returns the object stored in Owner's object property PropertyName, or nullptr.
	UObject* GetObjectProperty(const UObject* Owner, FName PropertyName);

	// Returns the nearest user widget that owns Widget, skipping Widget itself.
	UUserWidget* GetOwningUserWidget(const UWidget* Widget);

	// Calls the parameterless Blueprint function FunctionName on Object; returns false if it has none.
	bool CallBlueprintFunction(UObject* Object, FName FunctionName);
}
