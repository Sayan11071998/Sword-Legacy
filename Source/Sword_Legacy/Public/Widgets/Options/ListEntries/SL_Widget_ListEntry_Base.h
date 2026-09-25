#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_Widget_ListEntry_Base.generated.h"

class USL_ListDataObject_Base;
class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_Base : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	void NativeOnListEntryWidgetHovered(bool bWasHovered);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered, bool bIsEntryWidgetStillSelected);
	
protected:
	// ~ Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// ~ End IUserObjectListEntry Inter
	
	// ~ Begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	// ~ End UUserWidget Interface
	
	// The child class should override this function to handle the initialization needed. Super call is expected.
	virtual void OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject);
	
	// The child class should override this function to update the UI values after the data object has been modified. Super call is not needed.
	virtual void OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData, ESL_OptionsListDataModifyReason ModifyReason);

	void SelectThisEntryWidget();
	
	// The child widget blueprint should override this function for the gamepad interaction to function properly
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;
	
private:
	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> CommonText_SettingsDisplayName;
};