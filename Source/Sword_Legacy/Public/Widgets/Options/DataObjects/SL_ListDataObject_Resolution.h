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
};