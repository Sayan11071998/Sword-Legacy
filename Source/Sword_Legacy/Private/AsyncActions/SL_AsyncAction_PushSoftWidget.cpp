#include "AsyncActions/SL_AsyncAction_PushSoftWidget.h"
#include "Subsystems/SL_UISubsystem.h"
#include "Widgets/SL_Widget_Activatable_Base.h"

void USL_AsyncAction_PushSoftWidget::Activate()
{
	USL_UISubsystem* UISubsystem = USL_UISubsystem::Get(CachedOwningWorld.Get());
	
	UISubsystem->PushSoftWidgetToStackAsync(
		CachedWidgetStackTag,
		CachedSoftWidgetClass,
		[this](EAsyncPushWidgetState InPushState, USL_Widget_Activatable_Base* PushedWidget)
		{
			switch (InPushState)
			{
			case EAsyncPushWidgetState::OnCreatedBeforePush:
				PushedWidget->SetOwningPlayer(CachedOwningPC.Get());
				OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
				break;
			
			case EAsyncPushWidgetState::AfterPush:
				OnWidgetCreatedAfterPush.Broadcast(PushedWidget);
				
				if (bCachedFocusOnNewlyPushedWidget)
				{
					if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
					{
						WidgetToFocus->SetFocus();
					}
				}
				
				SetReadyToDestroy();
				
				break;
			
			default:
				break;
			}
		}
	);
}

USL_AsyncAction_PushSoftWidget* USL_AsyncAction_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject,
	APlayerController* OwningPlayerController, TSoftClassPtr<USL_Widget_Activatable_Base> InSoftWidgetClass,
	UPARAM(meta = (Categories = "UI.WidgetStack")) FGameplayTag InWidgetStackTag, bool bFocusOnNewlyPushedWidget)
{
	checkf(!InSoftWidgetClass.IsNull(), TEXT("PushSoftWidgetToStack was passed a null soft widget class"));
	
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			USL_AsyncAction_PushSoftWidget* Node = NewObject<USL_AsyncAction_PushSoftWidget>();
			
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;
			
			Node->RegisterWithGameInstance(World);
			
			return Node;
		}
	}
	
	return nullptr;
}