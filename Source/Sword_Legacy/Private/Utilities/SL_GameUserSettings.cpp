#include "Utilities/SL_GameUserSettings.h"

TObjectPtr<USL_GameUserSettings> USL_GameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<USL_GameUserSettings>(GEngine->GetGameUserSettings());
	}
	
	return nullptr;
}