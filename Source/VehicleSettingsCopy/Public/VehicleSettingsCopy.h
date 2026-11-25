#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVehicleSettingsCopy, Log, All);

// Module entry point for Vehicle Settings Copy.
class FVehicleSettingsCopyModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
