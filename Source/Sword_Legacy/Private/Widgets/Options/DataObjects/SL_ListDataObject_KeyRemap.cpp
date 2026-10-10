#include "Widgets/Options/DataObjects/SL_ListDataObject_KeyRemap.h"
#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"

void USL_ListDataObject_KeyRemap::InitKeyRemapData(TObjectPtr<UEnhancedInputUserSettings> InOwningInputUserSettings,
	TObjectPtr<UEnhancedPlayerMappableKeyProfile> InKeyProfile, ECommonInputType InDesiredInputKeyType,
	const FPlayerKeyMapping& InOwningPlayerKeyMapping)
{
	CachedOwningInputUserSettings = InOwningInputUserSettings;
	CachedOwningKeyProfile = InKeyProfile;
	CachedDesiredInputKeyType = InDesiredInputKeyType;
	CachedOwningMappingName = InOwningPlayerKeyMapping.GetMappingName();
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();

	ResolveChordKey();
}

FSlateBrush USL_ListDataObject_KeyRemap::GetIconFromCurrentKey() const
{
	return GetIconForKey(GetOwningKeyMapping()->GetCurrentKey());
}

FSlateBrush USL_ListDataObject_KeyRemap::GetIconFromChordKey() const
{
	return GetIconForKey(CachedChordKey);
}

bool USL_ListDataObject_KeyRemap::HasChordKey() const
{
	return CachedChordKey.IsValid();
}

void USL_ListDataObject_KeyRemap::BindNewInputKey(const FKey& InNewKey)
{
	check(CachedOwningInputUserSettings);
	
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	KeyArgs.NewKey = InNewKey;
	
	FGameplayTagContainer Container;
	
	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs, Container);
	CachedOwningInputUserSettings->SaveSettings();
	
	NotifyListDataModified(this);
}

bool USL_ListDataObject_KeyRemap::HasDefaultValue() const
{
	return GetOwningKeyMapping()->GetDefaultKey().IsValid();
}

bool USL_ListDataObject_KeyRemap::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && GetOwningKeyMapping()->IsCustomized();
}

bool USL_ListDataObject_KeyRemap::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		check(CachedOwningInputUserSettings);
		
		GetOwningKeyMapping()->ResetToDefault();
		
		CachedOwningInputUserSettings->SaveSettings();
		
		NotifyListDataModified(this, ESL_OptionsListDataModifyReason::ResetToDefault);
		
		return true;
	}
	
	return false;
}

FPlayerKeyMapping* USL_ListDataObject_KeyRemap::GetOwningKeyMapping() const
{
	check(CachedOwningKeyProfile);
	
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	
	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs);
}

FSlateBrush USL_ListDataObject_KeyRemap::GetIconForKey(const FKey& InKey) const
{
	FSlateBrush FoundBrush;

	if (!CachedOwningInputUserSettings || !InKey.IsValid()) return FoundBrush;

	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedOwningInputUserSettings->GetLocalPlayer());
	check(CommonInputSubsystem);

	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		FoundBrush,
		InKey,
		CachedDesiredInputKeyType,
		CommonInputSubsystem->GetCurrentGamepadName()
	);

	return FoundBrush;
}

void USL_ListDataObject_KeyRemap::ResolveChordKey()
{
	CachedChordKey = EKeys::Invalid;

	const FPlayerKeyMapping* Mapping = GetOwningKeyMapping();
	if (!Mapping) return;

	const UInputAction* Action = Mapping->GetAssociatedInputAction();
	if (!Action) return;

	const UInputTriggerChordAction* ChordTrigger = nullptr;

	for (const UInputTrigger* Trigger : Action->Triggers)
	{
		ChordTrigger = Cast<UInputTriggerChordAction>(Trigger);
		if (ChordTrigger && ChordTrigger->ChordAction) break;
		ChordTrigger = nullptr;
	}

	if (!ChordTrigger)
	{
		for (const TObjectPtr<const UInputMappingContext>& MappingContext : CachedOwningInputUserSettings->GetRegisteredInputMappingContexts())
		{
			if (!MappingContext) continue;

			for (const FEnhancedActionKeyMapping& SourceMapping : MappingContext->GetMappings())
			{
				if (SourceMapping.Action != Action || SourceMapping.Key != Mapping->GetDefaultKey()) continue;

				for (const UInputTrigger* Trigger : SourceMapping.Triggers)
				{
					ChordTrigger = Cast<UInputTriggerChordAction>(Trigger);
					if (ChordTrigger && ChordTrigger->ChordAction) break;
					ChordTrigger = nullptr;
				}
			}

			if (ChordTrigger) break;
		}
	}

	if (!ChordTrigger || !ChordTrigger->ChordAction) return;

	CachedChordKey = FindChordDisplayKey(ChordTrigger->ChordAction, Mapping->GetDefaultKey().IsGamepadKey());
}

FKey USL_ListDataObject_KeyRemap::FindChordDisplayKey(const UInputAction* ChordAction, bool bGamepadKey) const
{
	if (!ChordAction || !CachedOwningInputUserSettings) return EKeys::Invalid;

	FKey FallbackKey = EKeys::Invalid;

	for (const TObjectPtr<const UInputMappingContext>& MappingContext : CachedOwningInputUserSettings->GetRegisteredInputMappingContexts())
	{
		if (!MappingContext) continue;

		for (const FEnhancedActionKeyMapping& SourceMapping : MappingContext->GetMappings())
		{
			if (SourceMapping.Action != ChordAction || !SourceMapping.Key.IsValid()) continue;
			if (SourceMapping.Key.IsGamepadKey() != bGamepadKey) continue;

			if (!bGamepadKey && SourceMapping.Key == EKeys::W) return EKeys::W;

			if (!FallbackKey.IsValid())
			{
				FallbackKey = SourceMapping.Key;
			}
		}
	}

	return FallbackKey;
}