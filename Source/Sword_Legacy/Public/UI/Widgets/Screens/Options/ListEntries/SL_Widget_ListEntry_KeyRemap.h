#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "UI/Widgets/Screens/Options/ListEntries/SL_Widget_ListEntry_Base.h"
#include "SL_Widget_ListEntry_KeyRemap.generated.h"

class UCommonTextBlock;
class USL_ListDataObject_KeyRemap;
class USL_CommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_KeyRemap : public USL_Widget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	// ~ End UUserWidget Interface
	
	// ~ Begin USL_Widget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData, ESL_OptionsListDataModifyReason ModifyReason) override;
	virtual void NativeOnEntryReleased() override;
	// ~ End USL_Widget_ListEntry_Base Interface
	
private:
	void OnRemapKeyButtonClicked();
	void OnChordKeyButtonClicked();
	void OnResetKeyBindingButtonClicked();
	void OpenKeyRemapScreen();
	
	void OnKeyToRemapPressed(const FKey& PressedKey);
	void OnKeyRemapCanceled(const FString& CanceledReason);

	void RefreshChordDisplay();
	void AdvanceChordKeyCycle();
	void StopChordKeyCycle();
	
	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_RemapKey;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_ChordKey;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> CommonText_ChordPlus;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<USL_CommonButtonBase> CommonButton_ResetKeyBinding;
	
	// Member Variables
	UPROPERTY(Transient)
	TObjectPtr<USL_ListDataObject_KeyRemap> CachedOwningKeyRemapDataObject;

	FTimerHandle ChordKeyCycleTimer;
	int32 ChordKeyIndex = 0;
	bool bRemappingChordKey = false;
};