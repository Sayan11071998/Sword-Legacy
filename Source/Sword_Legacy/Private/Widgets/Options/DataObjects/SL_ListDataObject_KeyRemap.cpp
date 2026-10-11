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

	if (HasChordKey() && CachedOwningInputUserSettings &&
		!CachedOwningInputUserSettings->OnSettingsChanged.IsAlreadyBound(this, &USL_ListDataObject_KeyRemap::HandleUserSettingsChanged))
	{
		CachedOwningInputUserSettings->OnSettingsChanged.AddDynamic(this, &USL_ListDataObject_KeyRemap::HandleUserSettingsChanged);
	}
}

FSlateBrush USL_ListDataObject_KeyRemap::GetIconFromCurrentKey() const
{
	return GetIconForKey(GetOwningKeyMapping()->GetCurrentKey());
}

FSlateBrush USL_ListDataObject_KeyRemap::GetIconFromChordKey(int32 KeyIndex) const
{
	if (!CachedChordKeys.IsValidIndex(KeyIndex))
	{
		return FSlateBrush();
	}

	return GetIconForKey(CachedChordKeys[KeyIndex]);
}

bool USL_ListDataObject_KeyRemap::HasChordKey() const
{
	return CachedChordKeys.Num() > 0;
}

int32 USL_ListDataObject_KeyRemap::GetChordKeyCount() const
{
	return CachedChordKeys.Num();
}

bool USL_ListDataObject_KeyRemap::IsRemapLocked() const
{
	const FPlayerKeyMapping* Mapping = GetOwningKeyMapping();
	if (!Mapping) return false;

	return Mapping->GetCurrentKey() == EKeys::Mouse2D || Mapping->GetDefaultKey() == EKeys::Mouse2D;
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
		CachedOwningInputUserSettings->OnSettingsChanged.Broadcast(CachedOwningInputUserSettings);
		
		NotifyListDataModified(this, ESL_OptionsListDataModifyReason::ResetToDefault);
		
		return true;
	}
	
	return false;
}

void USL_ListDataObject_KeyRemap::BeginDestroy()
{
	if (CachedOwningInputUserSettings)
	{
		CachedOwningInputUserSettings->OnSettingsChanged.RemoveDynamic(this, &USL_ListDataObject_KeyRemap::HandleUserSettingsChanged);
	}

	Super::BeginDestroy();
}

void USL_ListDataObject_KeyRemap::HandleUserSettingsChanged(UEnhancedInputUserSettings* Settings)
{
	const TArray<FKey> PreviousChordKeys = CachedChordKeys;
	ResolveChordKey();

	if (PreviousChordKeys != CachedChordKeys)
	{
		NotifyListDataModified(this);
	}
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
	CachedChordKeys.Reset();

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

	CachedChordKeys = FindChordDisplayKeys(ChordTrigger->ChordAction, Mapping->GetDefaultKey().IsGamepadKey());
}

TArray<FKey> USL_ListDataObject_KeyRemap::FindChordDisplayKeys(const UInputAction* ChordAction, bool bGamepadKey) const
{
	TArray<FKey> DisplayKeys;
	if (!ChordAction || !CachedOwningInputUserSettings) return DisplayKeys;

	auto DirectionOrder = [](const FKey& Key)
	{
		if (Key == EKeys::W) return 0;
		if (Key == EKeys::A) return 1;
		if (Key == EKeys::S) return 2;
		if (Key == EKeys::D) return 3;
		return MAX_int32;
	};

	TArray<TPair<int32, FKey>> DirectionKeys;
	FKey FallbackKey = EKeys::Invalid;

	for (const TObjectPtr<const UInputMappingContext>& MappingContext : CachedOwningInputUserSettings->GetRegisteredInputMappingContexts())
	{
		if (!MappingContext) continue;

		for (const FEnhancedActionKeyMapping& SourceMapping : MappingContext->GetMappings())
		{
			if (SourceMapping.Action != ChordAction || !SourceMapping.Key.IsValid()) continue;
			if (SourceMapping.Key.IsGamepadKey() != bGamepadKey) continue;

			const int32 Order = DirectionOrder(SourceMapping.Key);
			if (!bGamepadKey && Order != MAX_int32)
			{
				DirectionKeys.Add(TPair<int32, FKey>(Order, FindCurrentKeyForMapping(SourceMapping)));
				continue;
			}

			if (!FallbackKey.IsValid())
			{
				FallbackKey = FindCurrentKeyForMapping(SourceMapping);
			}
		}
	}

	if (DirectionKeys.Num() > 0)
	{
		DirectionKeys.Sort([](const TPair<int32, FKey>& A, const TPair<int32, FKey>& B)
		{
			return A.Key < B.Key;
		});

		for (const TPair<int32, FKey>& DirectionKey : DirectionKeys)
		{
			DisplayKeys.AddUnique(DirectionKey.Value);
		}

		return DisplayKeys;
	}

	if (FallbackKey.IsValid())
	{
		DisplayKeys.Add(FallbackKey);
	}

	return DisplayKeys;
}

FKey USL_ListDataObject_KeyRemap::FindCurrentKeyForMapping(const FEnhancedActionKeyMapping& SourceMapping) const
{
	const FName MappingName = SourceMapping.GetMappingName();
	if (!CachedOwningKeyProfile || MappingName.IsNone()) return SourceMapping.Key;

	FPlayerMappableKeyQueryOptions QueryOptions;
	QueryOptions.MappingName = MappingName;
	QueryOptions.KeyToMatch = SourceMapping.Key;
	QueryOptions.bMatchBasicKeyTypes = true;

	TArray<FKey> MappedKeys;
	CachedOwningKeyProfile->QueryPlayerMappedKeys(QueryOptions, MappedKeys);

	// A unique mapping name, such as Move Forward, has one current key. That is the rebound key.
	if (MappedKeys.Num() == 1 && MappedKeys[0].IsValid())
	{
		return MappedKeys[0];
	}

	const FKeyMappingRow* MappingRow = CachedOwningKeyProfile->FindKeyMappingRow(MappingName);
	if (!MappingRow) return SourceMapping.Key;

	for (const FPlayerKeyMapping& PlayerMapping : MappingRow->Mappings)
	{
		if (PlayerMapping.GetDefaultKey() == SourceMapping.Key && PlayerMapping.GetCurrentKey().IsValid())
		{
			return PlayerMapping.GetCurrentKey();
		}
	}

	return SourceMapping.Key;
}