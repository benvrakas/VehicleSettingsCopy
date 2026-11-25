#include "UI/VSCPanel.h"
#include "VehicleSettingsCopy.h"
#include "VSCRemoteCallObject.h"
#include "UI/VSCWidgetUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/World.h"
#include "FGInventoryComponent.h"
#include "FGPlayerController.h"
#include "Resources/FGItemDescriptor.h"
#include "TimerManager.h"
#include "WheeledVehicles/FGWheeledVehicle.h"
#include "WheeledVehicles/FGWheeledVehicleIdentifier.h"

#define LOCTEXT_NAMESPACE "VehicleSettingsCopy"

namespace
{
	// The copied settings; shared by every vehicle menu until the game closes.
	struct FClipboard
	{
		bool bValid = false;
		TArray<FGuid> Route;
		TSubclassOf<UFGItemDescriptor> Fuel;
		FString SourceName;
	};
	FClipboard GClipboard;

	// Seconds after a paste when the route tab redraws, to catch the replicated route.
	constexpr float RefreshDelays[] = { 0.4f, 1.2f };
}

TSharedRef<SWidget> UVSCPanel::RebuildWidget()
{
	BuildTree();
	return Super::RebuildWidget();
}

void UVSCPanel::BuildTree()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);
	}
	if (WidgetTree->RootWidget)
	{
		return;
	}
	mRoot = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	WidgetTree->RootWidget = mRoot;

	mCopyButton = VSCWidgetUtils::CreateStandardButton(WidgetTree, LOCTEXT("Copy", "Copy"));
	mPasteButton = VSCWidgetUtils::CreateStandardButton(WidgetTree, LOCTEXT("Paste", "Paste"));
	for (UUserWidget* Button : { mCopyButton.Get(), mPasteButton.Get() })
	{
		if (Button)
		{
			if (UHorizontalBoxSlot* BoxSlot = mRoot->AddChildToHorizontalBox(Button))
			{
				BoxSlot->SetPadding(FMargin(4.f, 0.f, 0.f, 0.f));
			}
		}
	}
}

void UVSCPanel::NativeConstruct()
{
	Super::NativeConstruct();
	UButton* CopyInner = VSCWidgetUtils::GetInnerButton(mCopyButton);
	UButton* PasteInner = VSCWidgetUtils::GetInnerButton(mPasteButton);
	if (CopyInner && PasteInner)
	{
		CopyInner->OnClicked.AddUniqueDynamic(this, &UVSCPanel::HandleCopy);
		PasteInner->OnClicked.AddUniqueDynamic(this, &UVSCPanel::HandlePaste);
	}
	else
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("Vanilla standard button has no mButton; Copy/Paste clicks won't work"));
	}
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Copy/Paste panel shown for %s"), *GetNameSafe(GetIdentifier()));
}

AFGWheeledVehicleIdentifier* UVSCPanel::GetIdentifier() const
{
	return Cast<AFGWheeledVehicleIdentifier>(VSCWidgetUtils::GetObjectProperty(VSCWidgetUtils::GetOwningUserWidget(this), TEXT("mVehicleIdentifier")));
}

void UVSCPanel::HandleCopy()
{
	AFGWheeledVehicleIdentifier* Identifier = GetIdentifier();
	AFGWheeledVehicle* Vehicle = Identifier ? Identifier->GetOwnerVehicle() : nullptr;
	if (!Identifier || !Vehicle)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("Copy: no vehicle identifier on the route tab (%s)"), *GetNameSafe(Identifier));
		UVSCRemoteCallObject::ShowNotification(GetOwningPlayer<AFGPlayerController>(), LOCTEXT("CopyNoVehicle", "Nothing to copy: the vehicle isn't loaded."));
		return;
	}
	FClipboard Copied;
	Copied.bValid = true;
	Copied.Route = Identifier->GetVehicleRoute();
	Copied.SourceName = Identifier->GetVehicleName().ToString();
	if (const UFGInventoryComponent* Fuel = Vehicle->GetFuelInventory())
	{
		FInventoryStack Stack;
		if (Fuel->GetStackFromIndex(0, Stack) && Stack.HasItems())
		{
			Copied.Fuel = Stack.Item.GetItemClass();
		}
	}
	if (!Copied.Fuel)
	{
		Copied.Fuel = Identifier->GetFuelTypeDescriptor();
	}
	GClipboard = Copied;

	const FText FuelName = Copied.Fuel ? UFGItemDescriptor::GetItemName(Copied.Fuel) : LOCTEXT("NoFuel", "no fuel");
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Copied %d stops and fuel %s from %s"), Copied.Route.Num(), *GetNameSafe(Copied.Fuel), *Copied.SourceName);
	UVSCRemoteCallObject::ShowNotification(GetOwningPlayer<AFGPlayerController>(),
		FText::Format(LOCTEXT("Copied", "Copied {0} stops and fuel ({1})."), Copied.Route.Num(), FuelName));
}

void UVSCPanel::HandlePaste()
{
	AFGPlayerController* Controller = GetOwningPlayer<AFGPlayerController>();
	AFGWheeledVehicleIdentifier* Identifier = GetIdentifier();
	if (!GClipboard.bValid)
	{
		UVSCRemoteCallObject::ShowNotification(Controller, LOCTEXT("NothingCopied", "Nothing copied yet. Open a vehicle and press Copy first."));
		return;
	}
	if (!Identifier)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("Paste: no vehicle identifier on the route tab"));
		return;
	}
	UVSCRemoteCallObject* RCO = UVSCRemoteCallObject::GetLocal(this);
	if (!RCO)
	{
		UVSCRemoteCallObject::ShowNotification(Controller, LOCTEXT("PasteNoServerMod", "Paste isn't available: the server doesn't have Vehicle Settings Copy."));
		return;
	}
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Paste: sending %d stops and fuel %s from %s to %s"), GClipboard.Route.Num(), *GetNameSafe(GClipboard.Fuel),
		*GClipboard.SourceName, *Identifier->GetVehicleName().ToString());
	RCO->Server_PasteSettings(Identifier, GClipboard.Route, GClipboard.Fuel);
	if (UWorld* World = GetWorld())
	{
		for (const float Delay : RefreshDelays)
		{
			FTimerHandle Handle;
			World->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this]() { RefreshRouteTab(); }), Delay, false);
		}
	}
}

void UVSCPanel::RefreshRouteTab()
{
	UUserWidget* Tab = VSCWidgetUtils::GetOwningUserWidget(this);
	VSCWidgetUtils::CallBlueprintFunction(Tab, TEXT("PopulateCurrentStations"));
	VSCWidgetUtils::CallBlueprintFunction(Tab, TEXT("UpdateCurrentRoute"));
}

#undef LOCTEXT_NAMESPACE
