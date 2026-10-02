#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "SL_ListDataObject_Resolution.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_Resolution : public USL_ListDataObject_String
{
	GENERATED_BODY()
	
public:
	void InitResolutionValues();
	
protected:
	// ~ Begin USL_ListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// ~ End USL_ListDataObject_String Interface
	
private:
	FString ResToValueString(const FIntPoint& InResolution) const;
	FText ResToDisplayText(const FIntPoint& InResolution) const;
	
	UPROPERTY()
	FString MaximumAllowedResolution;
};