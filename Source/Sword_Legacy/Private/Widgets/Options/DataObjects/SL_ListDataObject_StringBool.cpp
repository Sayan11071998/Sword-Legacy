#include "Widgets/Options/DataObjects/SL_ListDataObject_StringBool.h"

void USL_ListDataObject_StringBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString, InNewTrueDisplayText);
	}
}

void USL_ListDataObject_StringBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString, InNewFalseDisplayText);
	}
}

void USL_ListDataObject_StringBool::SetTrueAsDefaultValue()
{
	SetDefaultValueFromString(TrueString);
}

void USL_ListDataObject_StringBool::SetFalseAsDefaultValue()
{
	SetDefaultValueFromString(FalseString);
}

void USL_ListDataObject_StringBool::OnDataObjectInitialized()
{
	TryInitBoolValues();
	
	Super::OnDataObjectInitialized();
}

void USL_ListDataObject_StringBool::TryInitBoolValues()
{
	if (!AvailableOptionsStringArray.Contains(TrueString))
	{
		AddDynamicOption(TrueString, FText::FromString(TEXT("ON")));
	}
	
	if (!AvailableOptionsStringArray.Contains(FalseString))
	{
		AddDynamicOption(FalseString, FText::FromString(TEXT("OFF")));
	}
}