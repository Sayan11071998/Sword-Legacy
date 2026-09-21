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
	
protected:
	TSharedPtr<FSL_OptionsDataInteractionHelper> DataDynamicGetter;
	TSharedPtr<FSL_OptionsDataInteractionHelper> DataDynamicSetter;
};