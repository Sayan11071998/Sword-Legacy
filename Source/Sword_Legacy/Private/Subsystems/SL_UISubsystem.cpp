#include "Subsystems/SL_UISubsystem.h"

TObjectPtr<USL_UISubsystem> USL_UISubsystem::Get(const TObjectPtr<UObject> WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		
		return UGameInstance::GetSubsystem<USL_UISubsystem>(World->GetGameInstance());
	}
	
	return nullptr;
}