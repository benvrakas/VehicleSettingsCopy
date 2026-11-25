#include "VSCRemoteCallObject.h"
#include "VehicleSettingsCopy.h"
#include "FGCharacterPlayer.h"
#include "FGInventoryComponent.h"
#include "FGPlayerController.h"
#include "Resources/FGItemDescriptor.h"
#include "UI/FGGameUI.h"
#include "WheeledVehicles/FGWheeledVehicle.h"
#include "WheeledVehicles/FGWheeledVehicleIdentifier.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "VehicleSettingsCopy"

namespace
{
	// Most stops a route can have.
	constexpr int32 MaxStops = 200;

	// Moves FuelClass from Player into slot 0 of Fuel, topping up the stack; a different fuel in the slot goes back to
	// Player first. Returns the number of items moved in, and describes what happened in Outcome.
	int32 FillFuelSlot(UFGInventoryComponent* Fuel, UFGInventoryComponent* Player, TSubclassOf<UFGItemDescriptor> FuelClass, FText& Outcome)
	{
		const FText Name = UFGItemDescriptor::GetItemName(FuelClass);
		if (!Fuel->IsItemAllowed(FuelClass, 0))
		{
			Outcome = FText::Format(LOCTEXT("FuelNotAllowed", "{0} can't be used as fuel by this vehicle."), Name);
			return 0;
		}
		FInventoryStack Current;
		Fuel->GetStackFromIndex(0, Current);
		const bool bHasCurrent = Current.HasItems();
		const bool bSame = bHasCurrent && Current.Item.GetItemClass() == FuelClass;
		const int32 Available = Player->GetNumItems(FuelClass);
		if (Available <= 0)
		{
			Outcome = bSame ? FText::Format(LOCTEXT("FuelNoneLeft", "You have no more {0}; the fuel slot is unchanged."), Name)
				: FText::Format(LOCTEXT("FuelNone", "You have no {0}; the fuel slot is unchanged."), Name);
			return 0;
		}
		if (bHasCurrent && !bSame)
		{
			if (!Player->HasEnoughSpaceForStack(Current))
			{
				Outcome = LOCTEXT("FuelNoRoom", "No room in your inventory for the fuel already in the slot; fuel unchanged.");
				return 0;
			}
			Player->AddStack(Current, true);
			Fuel->RemoveFromIndex(0, Current.NumItems);
			Current = FInventoryStack();
		}
		const int32 Room = FMath::Max(0, UFGItemDescriptor::GetStackSize(FuelClass) - (bSame ? Current.NumItems : 0));
		const int32 Take = FMath::Min(Room, Available);
		int32 Added = 0;
		if (Take > 0)
		{
			Player->Remove(FuelClass, Take);
			Added = Fuel->AddStackToIndex(0, FInventoryStack(Take, FuelClass), true);
			if (Added < Take)
			{
				Player->AddStack(FInventoryStack(Take - Added, FuelClass), true);
			}
		}
		Outcome = Added > 0 ? FText::Format(LOCTEXT("FuelFilled", "{0} x {1} moved into the fuel slot."), Added, Name)
			: FText::Format(LOCTEXT("FuelFull", "The fuel slot is already full of {0}."), Name);
		return Added;
	}
}

void UVSCRemoteCallObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UVSCRemoteCallObject, mForceNetField_UVSCRemoteCallObject);
}

UVSCRemoteCallObject* UVSCRemoteCallObject::GetLocal(const UObject* WorldContext)
{
	AFGPlayerController* Controller = WorldContext ? Cast<AFGPlayerController>(UGameplayStatics::GetPlayerController(WorldContext, 0)) : nullptr;
	if (!Controller)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("No local player controller to send the paste through"));
		return nullptr;
	}
	UVSCRemoteCallObject* RCO = Controller->GetRemoteCallObjectOfClass<UVSCRemoteCallObject>();
	if (!RCO)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("Remote call object not registered on the local player (is the mod installed on the server?)"));
	}
	return RCO;
}

void UVSCRemoteCallObject::ShowNotification(const AFGPlayerController* Controller, const FText& Message)
{
	if (UFGGameUI* UI = Controller ? Controller->GetGameUI() : nullptr)
	{
		UI->ShowTextNotification(Message);
	}
	else
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("No game UI to show: %s"), *Message.ToString());
	}
}

bool UVSCRemoteCallObject::Server_PasteSettings_Validate(AFGWheeledVehicleIdentifier* Identifier, const TArray<FGuid>& Route, TSubclassOf<UFGItemDescriptor> FuelClass)
{
	return Route.Num() <= MaxStops;
}

void UVSCRemoteCallObject::Server_PasteSettings_Implementation(AFGWheeledVehicleIdentifier* Identifier, const TArray<FGuid>& Route, TSubclassOf<UFGItemDescriptor> FuelClass)
{
	AFGPlayerController* Controller = GetOwnerPlayerController();
	const APlayerState* State = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
	const FString PlayerName = State ? State->GetPlayerName() : GetNameSafe(Controller);
	AFGWheeledVehicle* Vehicle = Identifier ? Identifier->GetOwnerVehicle() : nullptr;
	if (!Vehicle)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("Paste from %s rejected: %s has no vehicle"), *PlayerName, *GetNameSafe(Identifier));
		Client_Notify(LOCTEXT("NoVehicle", "Paste failed: the vehicle isn't loaded."));
		return;
	}

	Identifier->SetVehicleRoute(Route);
	Identifier->SanitizeVehicleRoute();
	const int32 Kept = Identifier->GetVehicleRoute().Num();
	const FText RouteText = FText::Format(LOCTEXT("RoutePasted", "Pasted {0} of {1} stops."), Kept, Route.Num());

	FText FuelText = LOCTEXT("NoFuelCopied", "No fuel was copied.");
	int32 FuelAdded = 0;
	if (FuelClass)
	{
		AFGCharacterPlayer* Player = Controller ? Cast<AFGCharacterPlayer>(Controller->GetControlledCharacter()) : nullptr;
		UFGInventoryComponent* PlayerInventory = Player ? Player->GetInventory() : nullptr;
		UFGInventoryComponent* FuelInventory = Vehicle->GetFuelInventory();
		if (PlayerInventory && FuelInventory)
		{
			FuelAdded = FillFuelSlot(FuelInventory, PlayerInventory, FuelClass, FuelText);
		}
		else
		{
			FuelText = LOCTEXT("NoInventory", "Fuel not moved: no inventory to take it from.");
		}
		Identifier->SetFuelTypeDescriptor(FuelClass);
	}
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Paste by %s onto %s: %d of %d stops kept, fuel %s, %d moved. %s"), *PlayerName, *Identifier->GetVehicleName().ToString(),
		Kept, Route.Num(), *GetNameSafe(FuelClass), FuelAdded, *FuelText.ToString());
	Client_Notify(FText::Format(LOCTEXT("PasteResult", "{0} {1}"), RouteText, FuelText));
}

void UVSCRemoteCallObject::Client_Notify_Implementation(const FText& Message)
{
	ShowNotification(GetOwnerPlayerController(), Message);
}

#undef LOCTEXT_NAMESPACE
