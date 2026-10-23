#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SL_ControlOrderTypes.generated.h"

USTRUCT(BlueprintType)
struct FSL_ControlOrderTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// Must match the control's display name
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	// Lower numbers are shown first. Keyboard and gamepad share this order.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SortOrder = 0;
};