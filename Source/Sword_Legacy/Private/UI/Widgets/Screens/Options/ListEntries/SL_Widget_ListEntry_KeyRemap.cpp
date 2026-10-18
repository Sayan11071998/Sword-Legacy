#include "UI/Widgets/Screens/Options/ListEntries/SL_Widget_ListEntry_KeyRemap.h"
#include "UI/Options/Data/ListDataObjects/SL_ListDataObject_KeyRemap.h"
#include "UI/Widgets/Components/SL_CommonButtonBase.h"
#include "CommonTextBlock.h"
#include "Subsystems/SL_UISubsystem.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "UI/Widgets/Screens/Options/SL_Widget_KeyRemapScreen.h"
#include "TimerManager.h"

void USL_Widget_ListEntry_KeyRemap::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CommonButton_RemapKey->OnClicked().AddUObject(this, &USL_Widget_ListEntry_KeyRemap::OnRemapKeyButtonClicked);
	CommonButton_ResetKeyBinding->OnClicked().AddUObject(this, &USL_Widget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked);

	if (CommonButton_ChordKey)
	{
		CommonButton_ChordKey->OnClicked().AddUObject(this, &USL_Widget_ListEntry_KeyRemap::OnChordKeyButtonClicked);
	}

	if (CommonText_ChordPlus)
	{
		CommonText_ChordPlus->SetText(FText::FromString(TEXT("+")));
	}
}

void USL_Widget_ListEntry_KeyRemap::OnOwningListDataObjectSet(TObjectPtr<USL_ListDataObject_Base> InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningKeyRemapDataObject = CastChecked<USL_ListDataObject_KeyRemap>(InOwningListDataObject);
	
	CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
	RefreshChordDisplay();
}

void USL_Widget_ListEntry_KeyRemap::NativeOnEntryReleased()
{
	StopChordKeyCycle();
	
	Super::NativeOnEntryReleased();
}

void USL_Widget_ListEntry_KeyRemap::NativeDestruct()
{
	StopChordKeyCycle();
	
	Super::NativeDestruct();
}

void USL_Widget_ListEntry_KeyRemap::OnOwningListDataObjectModified(USL_ListDataObject_Base* OwningModifiedData,
	ESL_OptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningKeyRemapDataObject)
	{
		CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
		RefreshChordDisplay();
	}
}

void USL_Widget_ListEntry_KeyRemap::OnRemapKeyButtonClicked()
{
	SelectThisEntryWidget();

	if (!CachedOwningKeyRemapDataObject) return;

	if (CachedOwningKeyRemapDataObject->IsRemapLocked())
	{
		USL_UISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
			ESL_ConfirmScreenType::Ok,
			FText::FromString(TEXT("Key Remap")),
			FText::FromString(
				TEXT("The ") +
				CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() +
				TEXT(" input cannot be modified.")
			),
			[](ESL_ConfirmScreenButtonType ClickedButton) { }
		);

		return;
	}

	bRemappingChordKey = false;
	OpenKeyRemapScreen();
}

void USL_Widget_ListEntry_KeyRemap::OnChordKeyButtonClicked()
{
	SelectThisEntryWidget();

	if (!CachedOwningKeyRemapDataObject || !CachedOwningKeyRemapDataObject->CanRemapChordKey()) return;

	bRemappingChordKey = true;
	OpenKeyRemapScreen();
}

void USL_Widget_ListEntry_KeyRemap::OpenKeyRemapScreen()
{
	USL_UISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		SL_GameplayTags::UI_WidgetStack_Modal,
		USL_FunctionLibrary::GetGameSoftWidgetClassByTag(SL_GameplayTags::UI_Widget_KeyRemapScreen),
		[this](EAsyncPushWidgetState PushState, USL_Widget_Activatable_Base* PushedWidget)
		{
			if (PushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				USL_Widget_KeyRemapScreen* CreatedKeyRemapScreen = CastChecked<USL_Widget_KeyRemapScreen>(PushedWidget);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindUObject(this, &USL_Widget_ListEntry_KeyRemap::OnKeyToRemapPressed);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeySelectCanceled.BindUObject(this, &USL_Widget_ListEntry_KeyRemap::OnKeyRemapCanceled);
				
				if (CachedOwningKeyRemapDataObject)
				{
					CreatedKeyRemapScreen->SetDesiredInputTypeToFilter(CachedOwningKeyRemapDataObject->GetDesiredInputKeyType());
				}
			}
		}
	);
}

void USL_Widget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked()
{
	SelectThisEntryWidget();
	
	if (!CachedOwningKeyRemapDataObject) return;
	
	// Check if the current key is already the default key. Display OK screen that says this is already the default key to the player.
	if (!CachedOwningKeyRemapDataObject->CanResetBackToDefaultValue())
	{
		USL_UISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
			ESL_ConfirmScreenType::Ok,
			FText::FromString(TEXT("Reset Key Mapping")),
			FText::FromString(
				TEXT("The key binding for ") +
				CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() +
				TEXT(" is already set to default.")
			),
			[](ESL_ConfirmScreenButtonType ClickedButton) { }
		);
		
		return;
	}
	
	// Reset the key binding back to default
	USL_UISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		ESL_ConfirmScreenType::YesNo,
		FText::FromString(TEXT("Reset Key Mapping")),
		FText::FromString(
			TEXT("Are you sure you want to reset the key for ") +
			CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() +
			TEXT(" ?")
		),
		[this](ESL_ConfirmScreenButtonType ClickedButton)
		{
			if (ClickedButton == ESL_ConfirmScreenButtonType::Confirmed)
			{
				CachedOwningKeyRemapDataObject->TryResetBackToDefaultValue();
			}
		}
	);
}

void USL_Widget_ListEntry_KeyRemap::OnKeyToRemapPressed(const FKey& PressedKey)
{
	const bool bChordRemap = bRemappingChordKey;
	bRemappingChordKey = false;

	if (!CachedOwningKeyRemapDataObject || !PressedKey.IsValid()) return;
	if (bChordRemap ? CachedOwningKeyRemapDataObject->IsUsingChordKey(PressedKey) : CachedOwningKeyRemapDataObject->IsUsingKey(PressedKey)) return;

	FText AssignedActionNames;
	const bool bKeyAlreadyAssigned = bChordRemap
		? CachedOwningKeyRemapDataObject->GetAssignedActionNamesForChordKey(PressedKey, AssignedActionNames)
		: CachedOwningKeyRemapDataObject->GetAssignedActionNamesForKey(PressedKey, AssignedActionNames);

	if (!bKeyAlreadyAssigned)
	{
		if (bChordRemap)
		{
			CachedOwningKeyRemapDataObject->BindNewChordKey(PressedKey);
		}
		else
		{
			CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
		}
		
		return;
	}

	const FString KeyName = PressedKey.GetDisplayName().ToString();
	const FString ActionNames = AssignedActionNames.ToString();

	USL_UISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		ESL_ConfirmScreenType::YesNo,
		FText::FromString(TEXT("Key Already Assigned")),
		FText::FromString(FString::Printf(
			TEXT("%s is already assigned to %s. Do you want to replace it? %s will be left with no key."),
			*KeyName,
			*ActionNames,
			*ActionNames
		)),
		[this, PressedKey, bChordRemap](ESL_ConfirmScreenButtonType ClickedButton)
		{
			if (ClickedButton == ESL_ConfirmScreenButtonType::Confirmed && CachedOwningKeyRemapDataObject)
			{
				if (bChordRemap)
				{
					CachedOwningKeyRemapDataObject->BindNewChordKey(PressedKey);
				}
				else
				{
					CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
				}
			}
		}
	);
}

void USL_Widget_ListEntry_KeyRemap::RefreshChordDisplay()
{
	StopChordKeyCycle();
	ChordKeyIndex = 0;

	const bool bShowChord = CachedOwningKeyRemapDataObject && CachedOwningKeyRemapDataObject->HasChordKey();
	const bool bCanRemapChord = bShowChord && CachedOwningKeyRemapDataObject->CanRemapChordKey();
	const int32 ChordKeyCount = bShowChord ? CachedOwningKeyRemapDataObject->GetChordKeyCount() : 0;

	if (CommonButton_ChordKey)
	{
		const ESlateVisibility ChordVisibility = !bShowChord
			? ESlateVisibility::Collapsed
			: (bCanRemapChord ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
		CommonButton_ChordKey->SetVisibility(ChordVisibility);
		CommonButton_ChordKey->SetIsInteractionEnabled(bCanRemapChord);

		if (bShowChord)
		{
			CommonButton_ChordKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromChordKey(ChordKeyIndex));
		}
	}

	if (CommonText_ChordPlus)
	{
		CommonText_ChordPlus->SetVisibility(bShowChord ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (ChordKeyCount > 1)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				ChordKeyCycleTimer,
				this,
				&USL_Widget_ListEntry_KeyRemap::AdvanceChordKeyCycle,
				1.f,
				true
			);
		}
	}
}

void USL_Widget_ListEntry_KeyRemap::AdvanceChordKeyCycle()
{
	if (!CachedOwningKeyRemapDataObject || !CommonButton_ChordKey) return;

	const int32 ChordKeyCount = CachedOwningKeyRemapDataObject->GetChordKeyCount();
	if (ChordKeyCount <= 1) return;

	ChordKeyIndex = (ChordKeyIndex + 1) % ChordKeyCount;
	CommonButton_ChordKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromChordKey(ChordKeyIndex));
}

void USL_Widget_ListEntry_KeyRemap::StopChordKeyCycle()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ChordKeyCycleTimer);
	}
}

void USL_Widget_ListEntry_KeyRemap::OnKeyRemapCanceled(const FString& CanceledReason)
{
	bRemappingChordKey = false;

	USL_UISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		ESL_ConfirmScreenType::Ok,
		FText::FromString(TEXT("Key Remap")),
		FText::FromString(CanceledReason),
		[](ESL_ConfirmScreenButtonType ClickedButton) { }
	);
}