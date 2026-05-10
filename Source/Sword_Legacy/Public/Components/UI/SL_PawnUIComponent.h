#pragma once

#include "CoreMinimal.h"
#include "Components/SL_PawnExtensionComponentBase.h"
#include "SL_PawnUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangedDelegate, float, NewPercent);

UCLASS()
class SWORD_LEGACY_API USL_PawnUIComponent : public USL_PawnExtensionComponentBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentHealthChanged;
};