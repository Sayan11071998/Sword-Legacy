#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SL_Widget_ListEntry_Base.generated.h"

class USL_ListDataObject_Base;
class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_ListEntry_Base : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	// ~ Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	// ~ End IUserObjectListEntry Inter
	
	// The child class should override this function to handle the initialization needed. Super call is expected.
	virtual void OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject);
	
private:
	// Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> CommonText_SettingsDisplayName;
};