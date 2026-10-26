#include "Subsystems/SL_LoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"

TSubclassOf<UUserWidget> USL_LoadingScreenSettings::GetLoadingScreenWidgetClassChecked() const
{
	if (SoftLoadingScreenWidgetClass.IsNull()) return nullptr;

	return SoftLoadingScreenWidgetClass.LoadSynchronous();
}