#include "Widgets/Options/DataObjects/SL_ListDataObject_Resolution.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utilities/SL_GameUserSettings.h"

void USL_ListDataObject_Resolution::InitResolutionValues()
{
	TArray<FIntPoint> AvailableResolutions;
	
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);
	
	AvailableResolutions.Sort(
		[](const FIntPoint& A, const FIntPoint& B) -> bool
		{
			return A.SizeSquared() < B.SizeSquared();
		}
	);
	
	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		AddDynamicOption(ResToValueString(Resolution), ResToDisplayText(Resolution));
	}
	
	MaximumAllowedResolution = ResToValueString(AvailableResolutions.Last());
	
	SetDefaultValueFromString(MaximumAllowedResolution);
}

void USL_ListDataObject_Resolution::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = ResToDisplayText(USL_GameUserSettings::Get()->GetScreenResolution());
	}
}

FString USL_ListDataObject_Resolution::ResToValueString(const FIntPoint& InResolution) const
{
	// Resolution Value from Dynamic Getter: (X=1536,Y=864)
	return FString::Printf(TEXT("X=%i,Y=%i"), InResolution.X, InResolution.Y);
}

FText USL_ListDataObject_Resolution::ResToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisplayString = FString::Printf(TEXT("%i X %i"), InResolution.X, InResolution.Y);
	
	return FText::FromString(DisplayString);
}