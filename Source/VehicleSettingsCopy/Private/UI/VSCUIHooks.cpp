#include "UI/VSCUIHooks.h"
#include "VehicleSettingsCopy.h"
#include "UI/VSCPanel.h"
#include "UI/VSCWidgetUtils.h"
#include "Engine/Engine.h"
#include "Patching/WidgetBlueprintHookManager.h"

namespace
{
	// Panel of the route tab the buttons go into.
	const TCHAR* ParentPanelName = TEXT("CanvasPanel_0");

	// X of the buttons' right edge, at the right end of the "Current route" header, and Y of their centre.
	constexpr float HeaderRight = 596.f;
	constexpr float HeaderCentre = 22.f;
}

void VSCUIHooks::Register(UObject* Outer, TArray<UWidgetBlueprintHookData*>& OutHooks)
{
#if WITH_EDITOR
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Editor build: UI hooks not registered"));
#else
	if (IsRunningDedicatedServer())
	{
		UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Dedicated server: UI hooks not registered"));
		return;
	}
	UWidgetBlueprintHookManager* Manager = GEngine ? GEngine->GetEngineSubsystem<UWidgetBlueprintHookManager>() : nullptr;
	if (!Manager)
	{
		UE_LOG(LogVehicleSettingsCopy, Error, TEXT("SML widget hook manager missing; no UI"));
		return;
	}
	UWidgetBlueprintHookData* Hook = NewObject<UWidgetBlueprintHookData>(Outer);
	Hook->WidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(VSCWidgetUtils::PathManagerClass));
	Hook->NewWidgetClass = UVSCPanel::StaticClass();
	Hook->NewWidgetName = TEXT("VSC_Panel");
	Hook->ParentWidgetName = ParentPanelName;
	Hook->ParentWidgetType = EWidgetBlueprintHookParentType::Direct;

	UWidgetBlueprintHookSlot_Canvas* Slot = NewObject<UWidgetBlueprintHookSlot_Canvas>(Outer);
	Slot->LayoutData.Anchors = FAnchors(0.f, 0.f, 0.f, 0.f);
	Slot->LayoutData.Alignment = FVector2D(1.f, 0.5f);
	Slot->LayoutData.Offsets = FMargin(HeaderRight, HeaderCentre, 0.f, 0.f);
	Slot->bAutoSize = true;
	Slot->ZOrder = 10;
	Hook->SlotConfiguration = Slot;

	if (!Hook->WidgetClass.LoadSynchronous())
	{
		UE_LOG(LogVehicleSettingsCopy, Error, TEXT("UI hook: vanilla widget %s not found"), VSCWidgetUtils::PathManagerClass);
		return;
	}
	if (!Hook->ResolveParentWidgetOnArchetype())
	{
		UE_LOG(LogVehicleSettingsCopy, Error, TEXT("UI hook: panel %s not found in %s"), ParentPanelName, VSCWidgetUtils::PathManagerClass);
		return;
	}
	Manager->RegisterWidgetBlueprintHook(Hook);
	OutHooks.Add(Hook);
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("UI hook added to %s in %s"), ParentPanelName, VSCWidgetUtils::PathManagerClass);
#endif
}
