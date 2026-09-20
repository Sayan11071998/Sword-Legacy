#include "Widgets/Options/ListEntries/SL_Widget_ListEntry_String.h"
#include "Widgets/Options/DataObjects/SL_ListDataObject_String.h"
#include "Widgets/Components/SL_CommonRotator.h"
#include "Widgets/Components/SL_CommonButtonBase.h"

#include "SL_DebugHelper.h"

void USL_Widget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CommonButton_PreviousOption->OnClicked().AddUObject(this, &USL_Widget_ListEntry_String::OnPreviousOptionButtonClicked);
	CommonButton_NextOption->OnClicked().AddUObject(this, &USL_Widget_ListEntry_String::OnNextOptionButtonClicked);
}

void USL_Widget_ListEntry_String::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningStringDataObject = CastChecked<USL_ListDataObject_String>(InOwningListDataObject);
	
	CommonRotator_AvailableOptions->PopulateTextLabels(CachedOwningStringDataObject->GetAvailableOptionsTextArray());
	CommonRotator_AvailableOptions->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentDisplayText());
}

void USL_Widget_ListEntry_String::OnPreviousOptionButtonClicked()
{
	Debug::Print(TEXT("Prev Option Button Clicked"));
}

void USL_Widget_ListEntry_String::OnNextOptionButtonClicked()
{
	Debug::Print(TEXT("Next Option Button Clicked"));
}