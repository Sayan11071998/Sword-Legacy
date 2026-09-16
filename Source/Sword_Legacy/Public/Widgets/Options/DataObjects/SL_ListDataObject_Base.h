#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SL_ListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const { return PropertyName; } \
	FORCEINLINE void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName; }

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_Base : public UObject
{
	GENERATED_BODY()
	
public:
	LIST_DATA_ACCESSOR(FName, DataID);
	LIST_DATA_ACCESSOR(FText, DataDisplayName);
	LIST_DATA_ACCESSOR(FText, DescriptionRichText);
	LIST_DATA_ACCESSOR(FText, DisabledRichText);
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage);
	LIST_DATA_ACCESSOR(TObjectPtr<USL_ListDataObject_Base>, ParentData);
	
	// Empty in the base class. Child class ListDataObjectCollection should override it. The function should return all the child data a tab has.
	virtual TArray<TObjectPtr<USL_ListDataObject_Base>> GetChildSettingData() const { return TArray<TObjectPtr<USL_ListDataObject_Base>>(); }
	
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
};