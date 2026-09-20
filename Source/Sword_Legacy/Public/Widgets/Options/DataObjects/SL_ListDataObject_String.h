#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Value.h"
#include "SL_ListDataObject_String.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_String : public USL_ListDataObject_Value
{
	GENERATED_BODY()
	
public:
	void AddDynamicOption(const FString& InStringValue, const FText& InDisplayText);
	
protected:
	UPROPERTY()
	FString CurrentStringValue;
	
	UPROPERTY()
	FText CurrentDisplayText;
	
	UPROPERTY()
	TArray<FString> AvailableOptionsStringArray;
	
	UPROPERTY()
	TArray<FText> AvailableOptionsTextArray;
};