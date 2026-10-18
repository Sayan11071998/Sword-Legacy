#include "UI/Options/Data/ListDataObjects/SL_ListDataObject_KeyRemap.h"
#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "EnhancedInputSubsystems.h"
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
	CachedOwningDisplayName = InOwningPlayerKeyMapping.GetDisplayName().ToString();
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();

	ResolveChordKey();

	if (const FPlayerKeyMapping* Mapping = GetOwningKeyMapping())
	{
		CachedDisplayedKey = Mapping->GetCurrentKey();
		if (ApplyKeyToLinkedMappings(CachedDisplayedKey))
		{
			SaveAndRebuildPlayerMappings();
		}
	}

	if (CachedOwningInputUserSettings &&
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

bool USL_ListDataObject_KeyRemap::CanRemapChordKey() const
{
	return !CachedChordMappingName.IsNone() && CachedChordKeys.Num() == 1 && CachedChordKeys[0].IsValid();
}

bool USL_ListDataObject_KeyRemap::IsRemapLocked() const
{
	const FPlayerKeyMapping* Mapping = GetOwningKeyMapping();
	if (!Mapping) return false;

	return Mapping->GetCurrentKey() == EKeys::Mouse2D || Mapping->GetDefaultKey() == EKeys::Mouse2D;
}

bool USL_ListDataObject_KeyRemap::IsUsingKey(const FKey& InKey) const
{
	const FPlayerKeyMapping* Mapping = GetOwningKeyMapping();
	
	return Mapping && Mapping->GetCurrentKey() == InKey;
}

bool USL_ListDataObject_KeyRemap::IsUsingChordKey(const FKey& InKey) const
{
	return CanRemapChordKey() && CachedChordKeys[0] == InKey;
}

bool USL_ListDataObject_KeyRemap::GetAssignedActionNamesForKey(const FKey& InNewKey, FText& OutActionNames) const
{
	return CollectActionNamesForKey(InNewKey, OutActionNames, false);
}

bool USL_ListDataObject_KeyRemap::GetAssignedActionNamesForChordKey(const FKey& InNewKey, FText& OutActionNames) const
{
	return CollectActionNamesForKey(InNewKey, OutActionNames, true);
}

bool USL_ListDataObject_KeyRemap::CollectActionNamesForKey(const FKey& InNewKey, FText& OutActionNames, bool bForChordKey) const
{
	OutActionNames = FText::GetEmpty();
	if (!InNewKey.IsValid() || !CachedOwningKeyProfile) return false;

	TArray<FString> ActionNames;

	for (const TPair<FName, FKeyMappingRow>& RowPair : CachedOwningKeyProfile->GetPlayerMappingRows())
	{
		for (const FPlayerKeyMapping& PlayerMapping : RowPair.Value.Mappings)
		{
			if (PlayerMapping.GetCurrentKey() != InNewKey) continue;
			
			if (bForChordKey)
			{
				if (PlayerMapping.GetMappingName() == CachedChordMappingName) continue;
			}
			else if (IsLinkedMapping(PlayerMapping))
			{
				continue;
			}

			const FString DisplayName = PlayerMapping.GetDisplayName().ToString();
			ActionNames.AddUnique(DisplayName.IsEmpty() ? PlayerMapping.GetMappingName().ToString() : DisplayName);
		}
	}

	if (ActionNames.IsEmpty()) return false;

	OutActionNames = FText::FromString(FString::Join(ActionNames, TEXT(", ")));
	return true;
}

void USL_ListDataObject_KeyRemap::BindNewInputKey(const FKey& InNewKey)
{
	check(CachedOwningInputUserSettings);

	UnbindOtherMappingsUsingKey(InNewKey, true);
	
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	KeyArgs.NewKey = InNewKey;
	
	FGameplayTagContainer Container;
	
	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs, Container);
	ApplyKeyToLinkedMappings(InNewKey);
	SaveAndRebuildPlayerMappings();
	
	NotifyListDataModified(this);
}

void USL_ListDataObject_KeyRemap::BindNewChordKey(const FKey& InNewKey)
{
	if (!CanRemapChordKey() || !CachedOwningInputUserSettings || !InNewKey.IsValid()) return;

	UnbindOtherMappingsUsingKey(InNewKey, false);

	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedChordMappingName;
	KeyArgs.Slot = CachedChordMappingSlot;
	KeyArgs.NewKey = InNewKey;

	FGameplayTagContainer FailureReason;
	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs, FailureReason);
	ApplyKeyToChordMappings(InNewKey);
	SaveAndRebuildPlayerMappings();

	NotifyListDataModified(this);
}

void USL_ListDataObject_KeyRemap::UnbindOtherMappingsUsingKey(const FKey& InKey, bool bKeepLinkedMappings)
{
	if (!InKey.IsValid() || !CachedOwningKeyProfile) return;

	struct FMappingToClear
	{
		FName MappingName = NAME_None;
		EPlayerMappableKeySlot Slot = EPlayerMappableKeySlot::Unspecified;
		FName HardwareDeviceId = NAME_None;
	};

	TArray<FMappingToClear> MappingsToClear;
	TSet<FString> SeenMappingIds;

	auto QueueMapping = [&MappingsToClear, &SeenMappingIds](const FPlayerKeyMapping& PlayerMapping)
	{
		const FString MappingId = FString::Printf(
			TEXT("%s|%d|%s"),
			*PlayerMapping.GetMappingName().ToString(),
			static_cast<int32>(PlayerMapping.GetSlot()),
			*PlayerMapping.GetHardwareDeviceId().HardwareDeviceIdentifier.ToString()
		);
		if (SeenMappingIds.Contains(MappingId)) return;

		SeenMappingIds.Add(MappingId);

		FMappingToClear MappingToClear;
		MappingToClear.MappingName = PlayerMapping.GetMappingName();
		MappingToClear.Slot = PlayerMapping.GetSlot();
		MappingToClear.HardwareDeviceId = PlayerMapping.GetHardwareDeviceId().HardwareDeviceIdentifier;
		MappingsToClear.Add(MappingToClear);
	};

	const TMap<FName, FKeyMappingRow>& MappingRows = CachedOwningKeyProfile->GetPlayerMappingRows();
	for (const TPair<FName, FKeyMappingRow>& RowPair : MappingRows)
	{
		for (const FPlayerKeyMapping& PlayerMapping : RowPair.Value.Mappings)
		{
			if (PlayerMapping.GetCurrentKey() != InKey) continue;
			if (bKeepLinkedMappings)
			{
				if (IsLinkedMapping(PlayerMapping)) continue;
			}
			else if (PlayerMapping.GetMappingName() == CachedChordMappingName)
			{
				continue;
			}

			QueueMapping(PlayerMapping);

			const FString ConflictingDisplayName = PlayerMapping.GetDisplayName().ToString();
			for (const TPair<FName, FKeyMappingRow>& LinkedRowPair : MappingRows)
			{
				for (const FPlayerKeyMapping& LinkedMapping : LinkedRowPair.Value.Mappings)
				{
					if (!IsMappingForThisDevice(LinkedMapping)) continue;

					const bool bSameMappingName = LinkedMapping.GetMappingName() == PlayerMapping.GetMappingName();
					const bool bSameDisplayName = !ConflictingDisplayName.IsEmpty() && LinkedMapping.GetDisplayName().ToString() == ConflictingDisplayName;
					if (bSameMappingName || bSameDisplayName)
					{
						QueueMapping(LinkedMapping);
					}
				}
			}
		}
	}

	for (const FMappingToClear& MappingToClear : MappingsToClear)
	{
		FMapPlayerKeyArgs KeyArgs;
		KeyArgs.MappingName = MappingToClear.MappingName;
		KeyArgs.Slot = MappingToClear.Slot;
		KeyArgs.HardwareDeviceId = MappingToClear.HardwareDeviceId;

		if (FPlayerKeyMapping* PlayerMapping = CachedOwningKeyProfile->FindKeyMapping(KeyArgs))
		{
			PlayerMapping->SetCurrentKey(EKeys::Invalid);
		}
	}
}

bool USL_ListDataObject_KeyRemap::ApplyKeyToLinkedMappings(const FKey& InKey)
{
	if (!CachedOwningKeyProfile || CachedOwningMappingName.IsNone()) return false;

	struct FMappingToUpdate
	{
		FName MappingName = NAME_None;
		EPlayerMappableKeySlot Slot = EPlayerMappableKeySlot::Unspecified;
		FName HardwareDeviceId = NAME_None;
	};

	TArray<FMappingToUpdate> MappingsToUpdate;
	for (const TPair<FName, FKeyMappingRow>& RowPair : CachedOwningKeyProfile->GetPlayerMappingRows())
	{
		for (const FPlayerKeyMapping& PlayerMapping : RowPair.Value.Mappings)
		{
			if (!IsLinkedMapping(PlayerMapping) || PlayerMapping.GetCurrentKey() == InKey) continue;

			FMappingToUpdate MappingToUpdate;
			MappingToUpdate.MappingName = PlayerMapping.GetMappingName();
			MappingToUpdate.Slot = PlayerMapping.GetSlot();
			MappingToUpdate.HardwareDeviceId = PlayerMapping.GetHardwareDeviceId().HardwareDeviceIdentifier;
			MappingsToUpdate.Add(MappingToUpdate);
		}
	}

	bool bChanged = false;
	for (const FMappingToUpdate& MappingToUpdate : MappingsToUpdate)
	{
		FMapPlayerKeyArgs KeyArgs;
		KeyArgs.MappingName = MappingToUpdate.MappingName;
		KeyArgs.Slot = MappingToUpdate.Slot;
		KeyArgs.HardwareDeviceId = MappingToUpdate.HardwareDeviceId;

		if (FPlayerKeyMapping* PlayerMapping = CachedOwningKeyProfile->FindKeyMapping(KeyArgs))
		{
			PlayerMapping->SetCurrentKey(InKey);
			bChanged = true;
		}
	}

	return bChanged;
}

bool USL_ListDataObject_KeyRemap::ApplyKeyToChordMappings(const FKey& InKey)
{
	if (!CachedOwningKeyProfile || CachedChordMappingName.IsNone()) return false;

	struct FMappingToUpdate
	{
		FName MappingName = NAME_None;
		EPlayerMappableKeySlot Slot = EPlayerMappableKeySlot::Unspecified;
		FName HardwareDeviceId = NAME_None;
	};

	TArray<FMappingToUpdate> MappingsToUpdate;
	for (const TPair<FName, FKeyMappingRow>& RowPair : CachedOwningKeyProfile->GetPlayerMappingRows())
	{
		for (const FPlayerKeyMapping& PlayerMapping : RowPair.Value.Mappings)
		{
			if (!IsMappingForThisDevice(PlayerMapping) || PlayerMapping.GetCurrentKey() == InKey) continue;

			const bool bSameMappingName = PlayerMapping.GetMappingName() == CachedChordMappingName;
			const bool bSameDisplayName = !CachedChordDisplayName.IsEmpty() && PlayerMapping.GetDisplayName().ToString() == CachedChordDisplayName;
			if (!bSameMappingName && !bSameDisplayName) continue;

			FMappingToUpdate MappingToUpdate;
			MappingToUpdate.MappingName = PlayerMapping.GetMappingName();
			MappingToUpdate.Slot = PlayerMapping.GetSlot();
			MappingToUpdate.HardwareDeviceId = PlayerMapping.GetHardwareDeviceId().HardwareDeviceIdentifier;
			MappingsToUpdate.Add(MappingToUpdate);
		}
	}

	bool bChanged = false;
	for (const FMappingToUpdate& MappingToUpdate : MappingsToUpdate)
	{
		FMapPlayerKeyArgs KeyArgs;
		KeyArgs.MappingName = MappingToUpdate.MappingName;
		KeyArgs.Slot = MappingToUpdate.Slot;
		KeyArgs.HardwareDeviceId = MappingToUpdate.HardwareDeviceId;

		if (FPlayerKeyMapping* PlayerMapping = CachedOwningKeyProfile->FindKeyMapping(KeyArgs))
		{
			PlayerMapping->SetCurrentKey(InKey);
			bChanged = true;
		}
	}

	return bChanged;
}

bool USL_ListDataObject_KeyRemap::IsMappingForThisDevice(const FPlayerKeyMapping& PlayerMapping) const
{
	const FKey DeviceKey = PlayerMapping.GetDefaultKey().IsValid() ? PlayerMapping.GetDefaultKey() : PlayerMapping.GetCurrentKey();
	const bool bMappingIsGamepad = DeviceKey.IsGamepadKey();
	const bool bRowIsGamepad = CachedDesiredInputKeyType == ECommonInputType::Gamepad;
	return bMappingIsGamepad == bRowIsGamepad;
}

bool USL_ListDataObject_KeyRemap::IsLinkedMapping(const FPlayerKeyMapping& PlayerMapping) const
{
	if (!IsMappingForThisDevice(PlayerMapping)) return false;
	if (PlayerMapping.GetMappingName() == CachedOwningMappingName) return true;

	const FString DisplayName = PlayerMapping.GetDisplayName().ToString();
	return !CachedOwningDisplayName.IsEmpty() && DisplayName == CachedOwningDisplayName;
}

void USL_ListDataObject_KeyRemap::SaveAndRebuildPlayerMappings()
{
	if (!CachedOwningInputUserSettings) return;

	CachedOwningInputUserSettings->SaveSettings();

	if (ULocalPlayer* LocalPlayer = CachedOwningInputUserSettings->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->RequestRebuildControlMappings();
		}
	}
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
		ApplyKeyToLinkedMappings(GetOwningKeyMapping()->GetCurrentKey());
		
		SaveAndRebuildPlayerMappings();
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
	FKey CurrentKey = EKeys::Invalid;
	if (const FPlayerKeyMapping* Mapping = GetOwningKeyMapping())
	{
		CurrentKey = Mapping->GetCurrentKey();
	}

	const bool bKeyChanged = CurrentKey != CachedDisplayedKey;
	CachedDisplayedKey = CurrentKey;

	const TArray<FKey> PreviousChordKeys = CachedChordKeys;
	ResolveChordKey();

	if (bKeyChanged || PreviousChordKeys != CachedChordKeys)
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
	auto MakeBlankBrush = []()
	{
		FSlateBrush BlankBrush;
		BlankBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		BlankBrush.TintColor = FLinearColor::Transparent;
		return BlankBrush;
	};

	if (!CachedOwningInputUserSettings || !InKey.IsValid()) return MakeBlankBrush();

	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedOwningInputUserSettings->GetLocalPlayer());
	check(CommonInputSubsystem);

	FSlateBrush FoundBrush;
	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
		FoundBrush,
		InKey,
		CachedDesiredInputKeyType,
		CommonInputSubsystem->GetCurrentGamepadName()
	);

	if (!bHasFoundBrush) return MakeBlankBrush();

	return FoundBrush;
}

void USL_ListDataObject_KeyRemap::ResolveChordKey()
{
	CachedChordKeys.Reset();
	CachedChordMappingName = NAME_None;
	CachedChordMappingSlot = EPlayerMappableKeySlot::Unspecified;
	CachedChordDisplayName.Reset();

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

	CachedChordKeys = FindChordDisplayKeys(
		ChordTrigger->ChordAction,
		Mapping->GetDefaultKey().IsGamepadKey(),
		CachedChordMappingName,
		CachedChordMappingSlot,
		CachedChordDisplayName
	);
}

TArray<FKey> USL_ListDataObject_KeyRemap::FindChordDisplayKeys(
	const UInputAction* ChordAction,
	bool bGamepadKey,
	FName& OutMappingName,
	EPlayerMappableKeySlot& OutSlot,
	FString& OutDisplayName) const
{
	TArray<FKey> DisplayKeys;
	OutMappingName = NAME_None;
	OutSlot = EPlayerMappableKeySlot::Unspecified;
	OutDisplayName.Reset();
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
	FName FallbackMappingName = NAME_None;
	EPlayerMappableKeySlot FallbackMappingSlot = EPlayerMappableKeySlot::Unspecified;
	FString FallbackDisplayName;

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
				const FKey ShownKey = FindCurrentKeyForMapping(SourceMapping);
				if (ShownKey.IsValid())
				{
					DirectionKeys.Add(TPair<int32, FKey>(Order, ShownKey));
				}
				continue;
			}

			if (!FallbackKey.IsValid())
			{
				FallbackKey = FindCurrentKeyForMapping(SourceMapping);
				FallbackMappingName = SourceMapping.GetMappingName();

				if (CachedOwningKeyProfile && !FallbackMappingName.IsNone())
				{
					if (const FKeyMappingRow* MappingRow = CachedOwningKeyProfile->FindKeyMappingRow(FallbackMappingName))
					{
						for (const FPlayerKeyMapping& PlayerMapping : MappingRow->Mappings)
						{
							if (PlayerMapping.GetDefaultKey() != SourceMapping.Key && PlayerMapping.GetCurrentKey() != FallbackKey) continue;

							FallbackMappingSlot = PlayerMapping.GetSlot();
							FallbackDisplayName = PlayerMapping.GetDisplayName().ToString();
							break;
						}
					}
				}
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
		OutMappingName = FallbackMappingName;
		OutSlot = FallbackMappingSlot;
		OutDisplayName = FallbackDisplayName;
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