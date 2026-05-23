#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "SL_BaseAnimInstance.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck) const;
};