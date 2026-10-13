#include "Input/SL_PlayerMappableKeyProfile.h"
#include "EnhancedActionKeyMapping.h"

int32 USL_PlayerMappableKeyProfile::GetPlayerMappedKeysForRebuildControlMappings(const FEnhancedActionKeyMapping& DefaultMapping, TArray<FKey>& OutKeys) const
{
	const int32 MappedKeyCount = Super::GetPlayerMappedKeysForRebuildControlMappings(DefaultMapping, OutKeys);
	if (MappedKeyCount > 0)
	{
		return MappedKeyCount;
	}

	const FKeyMappingRow* MappingRow = FindKeyMappingRow(DefaultMapping.GetMappingName());
	if (!MappingRow) return 0;

	for (const FPlayerKeyMapping& PlayerMapping : MappingRow->Mappings)
	{
		if (PlayerMapping.GetDefaultKey() == DefaultMapping.Key && PlayerMapping.IsCustomized() && !PlayerMapping.GetCurrentKey().IsValid())
		{
			OutKeys.Add(EKeys::Invalid);
			return OutKeys.Num();
		}
	}

	return 0;
}