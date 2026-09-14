#include "AsyncActions/SL_AsyncAction_PushConfirmScreen.h"
#include "Subsystems/SL_UISubsystem.h"

void USL_AsyncAction_PushConfirmScreen::Activate()
{
	USL_UISubsystem::Get(CachedOwningWorld.Get())->PushConfirmScreenToModalStackAsync(
		CachedScreenType,
		CachedScreenTitle,
		CachedScreenMessage,
		[this](ESL_ConfirmScreenButtonType ClickedButtonType)
		{
			OnButtonClicked.Broadcast(ClickedButtonType);
			
			SetReadyToDestroy();
		}
	);
}

USL_AsyncAction_PushConfirmScreen* USL_AsyncAction_PushConfirmScreen::PushConfirmScreen(
	const UObject* WorldContextObject, ESL_ConfirmScreenType ScreenType, FText InScreenTitle, FText InScreenMessage)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			USL_AsyncAction_PushConfirmScreen* Node = NewObject<USL_AsyncAction_PushConfirmScreen>();
			
			Node->CachedOwningWorld = World;
			Node->CachedScreenType = ScreenType;
			Node->CachedScreenTitle = InScreenTitle;
			Node->CachedScreenMessage = InScreenMessage;
			
			Node->RegisterWithGameInstance(World);
			
			return Node;
		}
	}
	
	return nullptr;
}