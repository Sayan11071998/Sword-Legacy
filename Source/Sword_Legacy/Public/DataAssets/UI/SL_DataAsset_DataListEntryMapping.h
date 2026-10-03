#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SL_DataAsset_DataListEntryMapping.generated.h"

class USL_Widget_ListEntry_Base;
class USL_ListDataObject_Base;

UCLASS()
class SWORD_LEGACY_API USL_DataAsset_DataListEntryMapping : public UDataAsset
{
	GENERATED_BODY()
	
public:
	TSubclassOf<USL_Widget_ListEntry_Base> FindEntryWidgetClassByDataObject(TObjectPtr<USL_ListDataObject_Base> InDataObject) const;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<USL_ListDataObject_Base>, TSubclassOf<USL_Widget_ListEntry_Base>> DataObjectListEntryMap;
};