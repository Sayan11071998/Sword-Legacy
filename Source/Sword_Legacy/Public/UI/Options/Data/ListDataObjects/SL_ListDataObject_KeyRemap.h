#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "UI/Options/Data/ListDataObjects/SL_ListDataObject_Base.h"
#include "SL_ListDataObject_KeyRemap.generated.h"

class UEnhancedPlayerMappableKeyProfile;
class UEnhancedInputUserSettings;
struct FEnhancedActionKeyMapping;

UCLASS()
class SWORD_LEGACY_API USL_ListDataObject_KeyRemap : public USL_ListDataObject_Base
{
	GENERATED_BODY()
	
public:
	void InitKeyRemapData(
		TObjectPtr<UEnhancedInputUserSettings> InOwningInputUserSettings,
		TObjectPtr<UEnhancedPlayerMappableKeyProfile> InKeyProfile,
		ECommonInputType InDesiredInputKeyType,
		const FPlayerKeyMapping& InOwningPlayerKeyMapping
	);
	
	FSlateBrush GetIconFromCurrentKey() const;
	FSlateBrush GetIconFromChordKey(int32 KeyIndex) const;

	bool HasChordKey() const;
	int32 GetChordKeyCount() const;
	bool CanRemapChordKey() const;
	bool IsRemapLocked() const;
	bool IsUsingKey(const FKey& InKey) const;
	bool IsUsingChordKey(const FKey& InKey) const;
	bool GetAssignedActionNamesForKey(const FKey& InNewKey, FText& OutActionNames) const;
	bool GetAssignedActionNamesForChordKey(const FKey& InNewKey, FText& OutActionNames) const;
	
	void BindNewInputKey(const FKey& InNewKey);
	void BindNewChordKey(const FKey& InNewKey);
	
	// ~ Begin USL_ListDataObject_Base Interface
	virtual bool HasDefaultValue() const override;
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	// ~ End USL_ListDataObject_Base Interface

	// ~ Begin UObject Interface
	virtual void BeginDestroy() override;
	// ~ End UObject Interface
	
private:
	FPlayerKeyMapping* GetOwningKeyMapping() const;
	FSlateBrush GetIconForKey(const FKey& InKey) const;
	void ResolveChordKey();
	TArray<FKey> FindChordDisplayKeys(
		const UInputAction* ChordAction,
		bool bGamepadKey,
		FName& OutMappingName,
		EPlayerMappableKeySlot& OutSlot,
		FString& OutDisplayName
	) const;
	FKey FindCurrentKeyForMapping(const FEnhancedActionKeyMapping& SourceMapping) const;
	void UnbindOtherMappingsUsingKey(const FKey& InKey, bool bKeepLinkedMappings);
	bool CollectActionNamesForKey(const FKey& InNewKey, FText& OutActionNames, bool bForChordKey) const;
	bool ApplyKeyToLinkedMappings(const FKey& InKey);
	bool ApplyKeyToChordMappings(const FKey& InKey);
	bool IsMappingForThisDevice(const FPlayerKeyMapping& PlayerMapping) const;
	bool IsLinkedMapping(const FPlayerKeyMapping& PlayerMapping) const;
	void SaveAndRebuildPlayerMappings();
	
	UFUNCTION()
	void HandleUserSettingsChanged(UEnhancedInputUserSettings* Settings);
	
	UPROPERTY(Transient)
	TObjectPtr<UEnhancedInputUserSettings> CachedOwningInputUserSettings;
	
	UPROPERTY(Transient)
	TObjectPtr<UEnhancedPlayerMappableKeyProfile> CachedOwningKeyProfile;
	
	UPROPERTY()
	ECommonInputType CachedDesiredInputKeyType;
	
	UPROPERTY()
	FName CachedOwningMappingName;

	UPROPERTY()
	FString CachedOwningDisplayName;
	
	UPROPERTY()
	EPlayerMappableKeySlot CachedOwningMappableKeySlot;

	UPROPERTY()
	TArray<FKey> CachedChordKeys;

	UPROPERTY()
	FName CachedChordMappingName;

	UPROPERTY()
	EPlayerMappableKeySlot CachedChordMappingSlot = EPlayerMappableKeySlot::Unspecified;

	UPROPERTY()
	FString CachedChordDisplayName;

	UPROPERTY()
	FKey CachedDisplayedKey;
	
public:
	FORCEINLINE ECommonInputType GetDesiredInputKeyType() const { return CachedDesiredInputKeyType; }
};