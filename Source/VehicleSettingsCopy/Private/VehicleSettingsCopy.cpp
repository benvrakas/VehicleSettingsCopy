#include "VehicleSettingsCopy.h"

DEFINE_LOG_CATEGORY(LogVehicleSettingsCopy);

void FVehicleSettingsCopyModule::StartupModule()
{
	UE_LOG(LogVehicleSettingsCopy, Log, TEXT("Vehicle Settings Copy module starting"));
}

void FVehicleSettingsCopyModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FVehicleSettingsCopyModule, VehicleSettingsCopy)
