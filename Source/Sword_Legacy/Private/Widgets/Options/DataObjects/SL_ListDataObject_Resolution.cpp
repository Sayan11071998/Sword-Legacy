#include "Widgets/Options/DataObjects/SL_ListDataObject_Resolution.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utilities/SL_OptionsDataInteractionHelper.h"

#include "SL_DebugHelper.h"

void USL_ListDataObject_Resolution::InitResolutionValues()
{
	TArray<FIntPoint> AvailableResolutions;
	
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);
	
	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		// Available Resolution: X=1920 Y=1080
		// Resolution Value from Dynamic Getter: (X=1536,Y=864)
		
		Debug::Print(TEXT("Available Resolution: ") + Resolution.ToString());
	}
}

void USL_ListDataObject_Resolution::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	
	if (DataDynamicGetter)
	{
		Debug::Print(TEXT("Resolution Value from Dynamic Getter: ") + DataDynamicGetter->GetValueAsString());
	}
}