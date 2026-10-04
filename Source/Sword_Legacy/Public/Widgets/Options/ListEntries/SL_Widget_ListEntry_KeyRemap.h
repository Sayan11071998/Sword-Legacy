#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "SL_Widget_ListEntry_KeyRemap.generated.h"

class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_KeyRemap : public USL_Widget_ListEntry_Base
{
	GENERATED_BODY()
	
private:
	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_RemapKey;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_ResetKeyBinding;
};