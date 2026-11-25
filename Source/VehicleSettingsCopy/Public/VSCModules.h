#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "VSCModules.generated.h"

class UWidgetBlueprintHookData;

// Vehicle Settings Copy's root game instance module; registers the remote call object and adds the UI.
UCLASS()
class VEHICLESETTINGSCOPY_API UVSCGameInstanceModule : public UGameInstanceModule
{
	GENERATED_BODY()

public:
	UVSCGameInstanceModule();

	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UWidgetBlueprintHookData>> mWidgetHooks;
};
