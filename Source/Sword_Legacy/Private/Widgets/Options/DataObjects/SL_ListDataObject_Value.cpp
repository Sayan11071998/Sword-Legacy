#include "Widgets/Options/DataObjects/SL_ListDataObject_Value.h"

void USL_ListDataObject_Value::SetDataDynamicGetter(const TSharedPtr<FSL_OptionsDataInteractionHelper>& InDynamicGetter)
{
	DataDynamicGetter = InDynamicGetter;
}

void USL_ListDataObject_Value::SetDataDynamicSetter(const TSharedPtr<FSL_OptionsDataInteractionHelper>& InDynamicSetter)
{
	DataDynamicSetter = InDynamicSetter;
}