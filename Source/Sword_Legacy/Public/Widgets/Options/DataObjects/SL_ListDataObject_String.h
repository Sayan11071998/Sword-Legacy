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
	void AdvanceToNextOption();
	void BackToPreviousOption();
	void OnRotatorInitiatedValueChanged(const FText& InNewSelectedText);
	
protected:
	// ~ Begin USL_ListDataObject_Base Interface
	virtual void OnDataObjectInitialized() override;
	// ~ End USL_ListDataObject_Base Interface
	
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);
	
	// ~ Begin USL_ListDataObject_Base Interface
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	// ~ End USL_ListDataObject_Base Interface
	
	UPROPERTY()
	FString CurrentStringValue;
	
	UPROPERTY()
	FText CurrentDisplayText;
	
	UPROPERTY()
	TArray<FString> AvailableOptionsStringArray;
	
	UPROPERTY()
	TArray<FText> AvailableOptionsTextArray;
	
public:
	FORCEINLINE const TArray<FText>& GetAvailableOptionsTextArray() const { return AvailableOptionsTextArray; }
	FORCEINLINE FText GetCurrentDisplayText() const { return CurrentDisplayText; }
};