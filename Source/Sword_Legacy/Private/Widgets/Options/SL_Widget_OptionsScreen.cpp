#include "Widgets/Options/SL_Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "SL_DebugHelper.h"
#include "Widgets/Options/SL_OptionsDataRegistry.h"

void USL_Widget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &USL_Widget_OptionsScreen::OnResetBoundActionTriggered)
			)
		);
	}
	
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this, &USL_Widget_OptionsScreen::OnBackBoundActionTriggered)
		)	
	);
}

TObjectPtr<USL_OptionsDataRegistry> USL_Widget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!CreatedOwningDataRegistry)
	{
		CreatedOwningDataRegistry = NewObject<USL_OptionsDataRegistry>();
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}
	
	checkf(CreatedOwningDataRegistry, TEXT("Data Registry for Options Screen is not valid."));
	
	return CreatedOwningDataRegistry;
}

void USL_Widget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("Reset Bound Action Triggered"));
}

void USL_Widget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}