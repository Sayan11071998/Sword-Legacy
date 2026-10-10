#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "SL_Widget_ListEntry_KeyRemap.generated.h"

class UCommonRichTextBlock;
class USL_ListDataObject_KeyRemap;
class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_KeyRemap : public USL_Widget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface
	
	// ~ Begin USL_Widget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData, ESL_OptionsListDataModifyReason ModifyReason) override;
	// ~ End USL_Widget_ListEntry_Base Interface
	
private:
	void OnRemapKeyButtonClicked();
	void OnResetKeyBindingButtonClicked();
	
	void OnKeyToRemapPressed(const FKey& PressedKey);
	void OnKeyRemapCanceled(const FString& CanceledReason);

	void RefreshChordDisplay();
	
	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_RemapKey;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_ChordKey;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonRichTextBlock> CommonText_ChordPlus;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_ResetKeyBinding;
	
	// Member Variables
	UPROPERTY(Transient)
	TObjectPtr<USL_ListDataObject_KeyRemap> CachedOwningKeyRemapDataObject;
};