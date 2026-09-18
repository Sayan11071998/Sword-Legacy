#include "Widgets/Options/SL_Widget_OptionsScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Options/SL_OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_Collection.h"
#include "Widgets/Components/SL_TabListWidgetBase.h"

#include "SL_DebugHelper.h"

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
	
	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this, &USL_Widget_OptionsScreen::OnOptionsTabSelected);
}

void USL_Widget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	for (USL_ListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection) continue;
		
		const FName TabID = TabCollection->GetDataID();
		
		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr) continue;
		
		TabListWidget_OptionsTabs->RequestRegisterTab(TabID, TabCollection->GetDataDisplayName());
	}
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

void USL_Widget_OptionsScreen::OnOptionsTabSelected(FName TabID)
{
	Debug::Print(TEXT("New Tab Selected. Tab ID: ") + TabID.ToString());
}