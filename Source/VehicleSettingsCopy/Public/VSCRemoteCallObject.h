#pragma once

#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"
#include "VSCRemoteCallObject.generated.h"

class AFGPlayerController;
class AFGWheeledVehicleIdentifier;
class UFGItemDescriptor;

// Carries a player's paste request to the server and the result message back.
UCLASS(NotBlueprintable)
class VEHICLESETTINGSCOPY_API UVSCRemoteCallObject : public UFGRemoteCallObject
{
	GENERATED_BODY()

public:
	// Sets the target vehicle's route and moves FuelClass from the player's inventory into its fuel slot.
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PasteSettings(AFGWheeledVehicleIdentifier* Identifier, const TArray<FGuid>& Route, TSubclassOf<UFGItemDescriptor> FuelClass);

	// Shows Message to the player as a game notification.
	UFUNCTION(Client, Reliable)
	void Client_Notify(const FText& Message);

	// Returns the local player's instance, or nullptr (logged) if the server doesn't run the mod.
	static UVSCRemoteCallObject* GetLocal(const UObject* WorldContext);

	// Shows Message through the local player's game UI.
	static void ShowNotification(const AFGPlayerController* Controller, const FText& Message);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated, Meta = (NoAutoJson))
	bool mForceNetField_UVSCRemoteCallObject = false;
};
