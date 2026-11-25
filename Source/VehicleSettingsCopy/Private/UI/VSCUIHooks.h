#pragma once

#include "CoreMinimal.h"

class UWidgetBlueprintHookData;

// Adds the Copy/Paste panel to the vanilla vehicle route tab.
namespace VSCUIHooks
{
	// Registers the hook and appends it to OutHooks so the caller keeps it alive; does nothing on a dedicated server.
	void Register(UObject* Outer, TArray<UWidgetBlueprintHookData*>& OutHooks);
}
