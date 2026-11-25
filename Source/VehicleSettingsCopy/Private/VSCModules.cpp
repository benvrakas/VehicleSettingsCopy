#include "VSCModules.h"
#include "VSCRemoteCallObject.h"
#include "UI/VSCUIHooks.h"

UVSCGameInstanceModule::UVSCGameInstanceModule()
{
	bRootModule = true;
	RemoteCallObjects.Add(UVSCRemoteCallObject::StaticClass());
}

void UVSCGameInstanceModule::DispatchLifecycleEvent(ELifecyclePhase Phase)
{
	if (Phase == ELifecyclePhase::POST_INITIALIZATION && mWidgetHooks.Num() == 0)
	{
		TArray<UWidgetBlueprintHookData*> Hooks;
		VSCUIHooks::Register(this, Hooks);
		mWidgetHooks.Append(Hooks);
	}
	Super::DispatchLifecycleEvent(Phase);
}
