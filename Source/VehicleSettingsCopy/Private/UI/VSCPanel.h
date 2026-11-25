#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VSCPanel.generated.h"

class AFGWheeledVehicleIdentifier;
class UHorizontalBox;

// "Copy" and "Paste" buttons added to the header of the vanilla vehicle route tab (BPW_VehiclePathManager).
UCLASS()
class UVSCPanel : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	void BuildTree();

	// Returns the identifier of the vehicle whose route tab this is, or nullptr.
	AFGWheeledVehicleIdentifier* GetIdentifier() const;

	// Makes the route tab redraw its stop list from the vehicle's current route.
	void RefreshRouteTab();

	UFUNCTION()
	void HandleCopy();

	UFUNCTION()
	void HandlePaste();

	UPROPERTY()
	TObjectPtr<UHorizontalBox> mRoot;

	UPROPERTY()
	TObjectPtr<UUserWidget> mCopyButton;

	UPROPERTY()
	TObjectPtr<UUserWidget> mPasteButton;
};
