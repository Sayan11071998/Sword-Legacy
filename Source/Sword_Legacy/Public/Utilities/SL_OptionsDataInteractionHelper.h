#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"

class USL_GameUserSettings;

class SWORD_LEGACY_API FSL_OptionsDataInteractionHelper
{
public:
	FSL_OptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath);
	
	FString GetValueAsString() const;
	void SetValueFromString(const FString& InStringValue);
	
private:
	FCachedPropertyPath CachedDynamicFunctionPath;
	TWeakObjectPtr<USL_GameUserSettings> CachedWeakGameUserSettings;
};