#pragma once

#include "CoreMinimal.h"
#include "SL_PawnEnumTypes.generated.h"

UENUM()
enum class ESL_ConfirmType : uint8
{
	Yes,
	No
};

UENUM()
enum class ESL_ValidType : uint8
{
	Valid,
	Invalid
};

UENUM(BlueprintType)
enum class ESL_ToggleDamageType : uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
};