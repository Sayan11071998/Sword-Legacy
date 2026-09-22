#pragma once

#include "CoreMinimal.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "UObject/NoExportTypes.h"
#include "SL_ListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const { return PropertyName; } \
	FORCEINLINE void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }

UCLASS(Abstract)
class SWORD_LEGACY_API USL_ListDataObject_Base : public UObject
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, USL_ListDataObject_Base*, ESL_OptionsListDataModifyReason);
	
	LIST_DATA_ACCESSOR(FName, DataID);
	LIST_DATA_ACCESSOR(FText, DataDisplayName);
	LIST_DATA_ACCESSOR(FText, DescriptionRichText);
	LIST_DATA_ACCESSOR(FText, DisabledRichText);
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage);
	LIST_DATA_ACCESSOR(TObjectPtr<USL_ListDataObject_Base>, ParentData);
	
	void InitDataObject();
	
	// Empty in the base class. Child class ListDataObjectCollection should override it. The function should return all the child data a tab has.
	virtual TArray<TObjectPtr<USL_ListDataObject_Base>> GetAllChildListData() const { return TArray<TObjectPtr<USL_ListDataObject_Base>>(); }
	virtual bool HasAnyChildListData() const { return false; }
	
	void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway) { bShouldApplyChangeImmediately = bShouldApplyRightAway; }
	
	// Delegate Variable
	FOnListDataModifiedDelegate OnListDataModified;
	
protected:
	// Empty in the base class. The child classes should override it to handle the initialization needed accordingly.
	virtual void OnDataObjectInitialized();
	
	virtual void NotifyListDataModified(TObjectPtr<USL_ListDataObject_Base> ModifiedData, ESL_OptionsListDataModifyReason ModifyReason = ESL_OptionsListDataModifyReason::DirectlyModified);
	
private:
	UPROPERTY()
	FName DataID;
	
	UPROPERTY()
	FText DataDisplayName;
	
	UPROPERTY()
	FText DescriptionRichText;
	
	UPROPERTY()
	FText DisabledRichText;
	
	UPROPERTY()
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;
	
	UPROPERTY(Transient)
	TObjectPtr<USL_ListDataObject_Base> ParentData;
	
	UPROPERTY()
	bool bShouldApplyChangeImmediately = false;
};