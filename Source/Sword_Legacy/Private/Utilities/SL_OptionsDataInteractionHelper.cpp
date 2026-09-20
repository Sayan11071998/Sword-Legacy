#include "Utilities/SL_OptionsDataInteractionHelper.h"
#include "Utilities/SL_GameUserSettings.h"

FSL_OptionsDataInteractionHelper::FSL_OptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath)
	: CachedDynamicFunctionPath(InSetterOrGetterFuncPath)
{
	CachedWeakGameUserSettings = USL_GameUserSettings::Get();
}

FString FSL_OptionsDataInteractionHelper::GetValueAsString() const
{
	FString OutStringValue;
	
	PropertyPathHelpers::GetPropertyValueAsString(
		CachedWeakGameUserSettings.Get(),
		CachedDynamicFunctionPath,
		OutStringValue	
	);
	
	return OutStringValue;
}

void FSL_OptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue)
{
	PropertyPathHelpers::SetPropertyValueFromString(
		CachedWeakGameUserSettings.Get(),
		CachedDynamicFunctionPath,
		InStringValue
	);
}