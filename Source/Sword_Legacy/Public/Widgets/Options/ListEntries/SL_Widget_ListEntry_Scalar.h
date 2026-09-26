#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "SL_Widget_ListEntry_Scalar.generated.h"

class UAnalogSlider;
class UCommonNumericTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_Scalar : public USL_Widget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interfac
	// ~ Begin USL_Widget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData, ESL_OptionsListDataModifyReason ModifyReason) override;
	// ~ End USL_Widget_ListEntry_Base Interface
	
private:
	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonNumericTextBlock> CommonNumeric_SettingValue;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UAnalogSlider> AnalogSlider_SettingSlider;
};