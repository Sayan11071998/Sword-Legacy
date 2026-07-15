#include "Subsystems/SL_UISubsystem.h"

#include "SL_DebugHelper.h"

TObjectPtr<USL_UISubsystem> USL_UISubsystem::Get(const TObjectPtr<UObject> WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
		
		return UGameInstance::GetSubsystem<USL_UISubsystem>(World->GetGameInstance());
	}
	
	return nullptr;
}

bool USL_UISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		
		GetDerivedClasses(GetClass(), FoundClasses);
		
		return FoundClasses.IsEmpty();
	}
	
	return false;
}

void USL_UISubsystem::RegisterCreatedPrimaryLayoutWidget(USL_Widget_PrimaryLayout* InCreatedWidget)
{
	check(InCreatedWidget);
	
	CreatedPrimaryLayout = InCreatedWidget;
	
	Debug::Print(TEXT("Primary Layout Widget Stored"));
}