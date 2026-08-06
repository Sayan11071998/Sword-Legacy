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

UENUM()
enum class ESL_SuccessType : uint8
{
	Successful,
	Failed
};

UENUM()
enum class ESL_CountDownActionInput : uint8
{
	Start,
	Cancel
};

UENUM()
enum class ESL_CountDownActionOutput : uint8
{
	Updated,
	Completed,
	Cancelled
};

UENUM(BlueprintType)
enum class ESL_ConfirmScreenType : uint8
{
	Ok,
	YesNo,
	OkCancel,
	Unknown UMETA(Hidden)
};

UENUM()
enum class ESL_ConfirmScreenButtonType : uint8
{
	Confirmed,
	Cancelled,
	Closed,
	Unknown UMETA(Hidden)
};