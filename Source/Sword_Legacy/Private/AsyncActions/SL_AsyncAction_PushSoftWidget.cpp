#include "AsyncActions/SL_AsyncAction_PushSoftWidget.h"

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
			
			Node->RegisterWithGameInstance(World);
			
			return Node;
		}
	}
	
	return nullptr;
}