#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SL_CreditTypes.generated.h"

USTRUCT(BlueprintType)
struct FSL_CreditTableRow : public FTableRowBase
{
	GENERATED_BODY()

	// Department or heading. Repeated rows with the same section stay under one heading.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Section;

	// Job title under the section. Leave empty when the section name is the title.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PersonName;
};