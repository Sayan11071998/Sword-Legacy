#pragma once

#include "CoreMinimal.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "Widgets/SL_Widget_Activatable_Base.h"
#include "SL_Widget_OptionsScreen.generated.h"

class USL_ListDataObject_Base;
class USL_Widget_OptionsDetailsView;
class USL_TabListWidgetBase;
class USL_OptionsDataRegistry;
class USL_CommonListView;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class SWORD_LEGACY_API USL_Widget_OptionsScreen : public USL_Widget_Activatable_Base
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// ~ End UUserWidget Interface
	
	// ~ Begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	// ~ End UCommonActivatableWidget Interface
	
private:
	TObjectPtr<USL_OptionsDataRegistry> GetOrCreateDataRegistry();
	
	void OnResetBoundActionTriggered();
	void OnBackBoundActionTriggered();
	
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	void OnListViewItemSelected(UObject* InSelectedItem);
	
	void OnListViewListDataModified(USL_ListDataObject_Base* ModifiedData, ESL_OptionsListDataModifyReason ModifyReason);
	
	FString TryGetEntryWidgetClassName(TObjectPtr<UObject> InOwningListItem) const;
	
	UFUNCTION()
	void OnOptionsTabSelected(FName TabID);
	
	// Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_TabListWidgetBase> TabListWidget_OptionsTabs;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_CommonListView> CommonListView_OptionsList;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USL_Widget_OptionsDetailsView> DetailsView_ListEntryInfo;
	
	// Handle the creation of data registry in options screen. Direct access to this variable is forbidden.
	UPROPERTY(Transient)
	TObjectPtr<USL_OptionsDataRegistry> CreatedOwningDataRegistry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	
	UPROPERTY()
	FUIActionBindingHandle ResetActionHandle;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<USL_ListDataObject_Base>> ResettableDataArray;
	
	UPROPERTY()
	bool bIsResettingData = false;
};