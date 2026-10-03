#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "SL_CommonRotator.generated.h"

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_CommonRotator : public UCommonRotator
{
	GENERATED_BODY()
	
public:
	void SetSelectedOptionByText(const FText& InTextOption);
};