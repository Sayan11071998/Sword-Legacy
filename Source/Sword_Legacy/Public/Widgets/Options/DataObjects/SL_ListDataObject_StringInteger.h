#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "SL_ListDataObject_StringInteger.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_StringInteger : public USL_ListDataObject_String
{
	GENERATED_BODY()
	
public:
	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);
	
protected:
	// ~ Begin USL_ListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// ~ End USL_ListDataObject_String Interface
	
	// ~ Begin USL_ListDataObject_Base Interface
	virtual  void OnEditDependencyDataModified(USL_ListDataObject_Base* ModifiedDependencyData, ESL_OptionsListDataModifyReason ModifyReason = ESL_OptionsListDataModifyReason::DirectlyModified) override;
	// ~ End USL_ListDataObject_Base Interface
};