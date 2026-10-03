#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Base.h"
#include "SL_ListDataObject_Collection.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_Collection : public USL_ListDataObject_Base
{
	GENERATED_BODY()
	
public:
	void AddChildListData(TObjectPtr<USL_ListDataObject_Base> InChildListData);
	
	// ~ Begin USL_ListDataObject_Base Interface
	virtual TArray<TObjectPtr<USL_ListDataObject_Base>> GetAllChildListData() const override;
	virtual bool HasAnyChildListData() const override;
	// ~ End USL_ListDataObject_Base Interface

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<USL_ListDataObject_Base>> ChildListDataArray;
};