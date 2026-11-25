#include "UI/VSCWidgetUtils.h"
#include "VehicleSettingsCopy.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UObject/UnrealType.h"

namespace VSCWidgetUtils
{
	const TCHAR* StandardButtonClass = TEXT("/Game/FactoryGame/Interface/UI/InGame/-Shared/Widget_StandardButton.Widget_StandardButton_C");
	const TCHAR* PathManagerClass = TEXT("/Game/FactoryGame/Buildable/Vehicle/BPW_VehiclePathManager.BPW_VehiclePathManager_C");
}

UUserWidget* VSCWidgetUtils::CreateStandardButton(UWidgetTree* Tree, const FText& Text)
{
	UClass* Class = Tree ? LoadClass<UUserWidget>(nullptr, StandardButtonClass) : nullptr;
	if (!Class)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("Vanilla button class %s not found"), StandardButtonClass);
		return nullptr;
	}
	UUserWidget* Button = Tree->ConstructWidget<UUserWidget>(Class);
	if (Button)
	{
		SetWidgetText(Button, Text);
	}
	return Button;
}

bool VSCWidgetUtils::SetWidgetText(UUserWidget* Widget, const FText& Text)
{
	UFunction* Function = Widget ? Widget->FindFunction(TEXT("SetText")) : nullptr;
	if (Function)
	{
		FTextProperty* TextParam = nullptr;
		for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
		{
			if (FTextProperty* AsText = CastField<FTextProperty>(*It))
			{
				TextParam = AsText;
				break;
			}
		}
		if (TextParam)
		{
			uint8* Params = static_cast<uint8*>(FMemory_Alloca(FMath::Max<int32>(Function->ParmsSize, 1)));
			FMemory::Memzero(Params, Function->ParmsSize);
			for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
			{
				It->InitializeValue_InContainer(Params);
			}
			TextParam->SetPropertyValue_InContainer(Params, Text);
			Widget->ProcessEvent(Function, Params);
			for (TFieldIterator<FProperty> It(Function); It && It->HasAnyPropertyFlags(CPF_Parm); ++It)
			{
				It->DestroyValue_InContainer(Params);
			}
			return true;
		}
	}
	if (UTextBlock* TextObject = Cast<UTextBlock>(GetObjectProperty(Widget, TEXT("mTextObject"))))
	{
		TextObject->SetText(Text);
		return true;
	}
	UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("%s has no SetText; text \"%s\" not shown"), *GetNameSafe(Widget), *Text.ToString());
	return false;
}

UButton* VSCWidgetUtils::GetInnerButton(UUserWidget* StandardButton)
{
	return Cast<UButton>(GetObjectProperty(StandardButton, TEXT("mButton")));
}

UObject* VSCWidgetUtils::GetObjectProperty(const UObject* Owner, FName PropertyName)
{
	FObjectPropertyBase* Property = Owner ? FindFProperty<FObjectPropertyBase>(Owner->GetClass(), PropertyName) : nullptr;
	return Property ? Property->GetObjectPropertyValue_InContainer(Owner) : nullptr;
}

UUserWidget* VSCWidgetUtils::GetOwningUserWidget(const UWidget* Widget)
{
	for (UObject* Outer = Widget ? Widget->GetOuter() : nullptr; Outer; Outer = Outer->GetOuter())
	{
		if (UUserWidget* AsUser = Cast<UUserWidget>(Outer))
		{
			return AsUser;
		}
	}
	return nullptr;
}

bool VSCWidgetUtils::CallBlueprintFunction(UObject* Object, FName FunctionName)
{
	UFunction* Function = Object ? Object->FindFunction(FunctionName) : nullptr;
	if (!Function || Function->ParmsSize > 0)
	{
		UE_LOG(LogVehicleSettingsCopy, Warning, TEXT("%s has no parameterless function %s"), *GetNameSafe(Object), *FunctionName.ToString());
		return false;
	}
	Object->ProcessEvent(Function, nullptr);
	return true;
}
