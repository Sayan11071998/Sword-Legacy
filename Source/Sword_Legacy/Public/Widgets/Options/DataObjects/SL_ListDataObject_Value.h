#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "SL_ListDataObject_Value.generated.h"

class FSL_OptionsDataInteractionHelper;

UCLASS(Abstract)
class SWORD_LEGACY_API USL_ListDataObject_Value : public USL_ListDataObject_Base
{
	GENERATED_BODY()
	
public:
	void SetDataDynamicGetter(const TSharedPtr<FSL_OptionsDataInteractionHelper>& InDynamicGetter);
	void SetDataDynamicSetter(const TSharedPtr<FSL_OptionsDataInteractionHelper>& InDynamicSetter);
	
	void SetDefaultValueFromString(const FString& InDefaultValue) { DefaultStringValue = InDefaultValue; }
	
	// ~ Begin USL_ListDataObject_Base Interface
	virtual bool HasDefaultValue() const override { return DefaultStringValue.IsSet(); }
	// ~ End USL_ListDataObject_Base Interface
	
protected:
	FString GetDefaultValueAsString() const { return DefaultStringValue.GetValue(); }
	
	TSharedPtr<FSL_OptionsDataInteractionHelper> DataDynamicGetter;
	TSharedPtr<FSL_OptionsDataInteractionHelper> DataDynamicSetter;
	
private:
	TOptional<FString> DefaultStringValue;
};