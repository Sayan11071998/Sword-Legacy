#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_String.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Widgets/Components/SL_CommonRotator.h"
#include "Widgets/Components/SL_CommonButtonBase.h"
#include "CommonInputSubsystem.h"

void USL_Widget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CommonButton_PreviousOption->OnClicked().AddUObject(this, &USL_Widget_ListEntry_String::OnPreviousOptionButtonClicked);
	CommonButton_NextOption->OnClicked().AddUObject(this, &USL_Widget_ListEntry_String::OnNextOptionButtonClicked);
	
	CommonRotator_AvailableOptions->OnClicked().AddLambda(
		[this]()
		{
			SelectThisEntryWidget();
		}
	);
	
	CommonRotator_AvailableOptions->OnRotatedEvent.AddUObject(this, &USL_Widget_ListEntry_String::OnRotatorValueChanged);
}

void USL_Widget_ListEntry_String::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningStringDataObject = CastChecked<USL_ListDataObject_String>(InOwningListDataObject);
	
	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
}

void USL_Widget_ListEntry_String::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningStringDataObject)
	{
		CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
	}
}

void USL_Widget_ListEntry_String::OnPreviousOptionButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->BackToPreviousOption();
	}
	
	SelectThisEntryWidget();
}

void USL_Widget_ListEntry_String::OnNextOptionButtonClicked()
{
	if (CachedOwningStringDataObject)
	{
		CachedOwningStringDataObject->AdvanceToNextOption();
	}
	
	SelectThisEntryWidget();
}

void USL_Widget_ListEntry_String::OnRotatorValueChanged(int32 Value, bool bUserInitiated)
{
	if (!CachedOwningStringDataObject) return;
	
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	
	if (!CommonInputSubsystem || !bUserInitiated) return;
	
	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		CachedOwningStringDataObject->OnRotatorInitiatedValueChanged(CommonRotator_AvailableOptions->GetSelectedText());
	}
}