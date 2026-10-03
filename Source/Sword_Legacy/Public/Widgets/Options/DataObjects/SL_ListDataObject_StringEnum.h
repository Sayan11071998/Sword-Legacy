#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "SL_ListDataObject_StringEnum.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_StringEnum : public USL_ListDataObject_String
{
	GENERATED_BODY()
	
public:
	template<typename EnumType>
	void AddEnumOption(EnumType InEnumOption, const FText& InDisplayText)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);
		
		AddDynamicOption(ConvertedEnumString, InDisplayText);
	}
	
	template<typename EnumType>
	EnumType GetCurrentValueAsEnum() const
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		
		return (EnumType)StaticEnumOption->GetValueByNameString(CurrentStringValue);
	}
	
	template<typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumOption)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);
		
		SetDefaultValueFromString(ConvertedEnumString);
	}
};