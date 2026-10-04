#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "SL_ListDataObject_StringBool.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_StringBool : public USL_ListDataObject_String
{
	GENERATED_BODY()
	
public:
	void OverrideTrueDisplayText(const FText& InNewTrueDisplayText);
	void OverrideFalseDisplayText(const FText& InNewFalseDisplayText);
	void SetTrueAsDefaultValue();
	void SetFalseAsDefaultValue();
	
protected:
	// ~ Begin USL_ListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// ~ End USL_ListDataObject_String Interface
	
private:
	void TryInitBoolValues();
	
	const FString TrueString = TEXT("true");
	const FString FalseString = TEXT("false");
};