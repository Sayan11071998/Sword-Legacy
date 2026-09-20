#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "SL_Widget_ListEntry_String.generated.h"

class USL_ListDataObject_String;
class USL_CommonRotator;
class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_String : public USL_Widget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin USL_Widget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject) override;
	// ~ End USL_Widget_ListEntry_Base Interface
	
private:
	// Begin Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_PreviousOption;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonRotator> CommonRotator_AvailableOptions;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_NextOption;
	// End Bound Widgets
	
	UPROPERTY(Transient)
	TObjectPtr<USL_ListDataObject_String> CachedOwningStringDataObject;
};