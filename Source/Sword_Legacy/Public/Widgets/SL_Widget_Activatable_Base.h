#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "SL_Widget_Activatable_Base.generated.h"

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_Activatable_Base : public UCommonActivatableWidget
{
	GENERATED_BODY()
};