#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/SL_BaseAnimInstance.h"
#include "SL_PlayerLinkedAnimLayer.generated.h"

class USL_PlayerAnimInstance;

UCLASS()
class SWORD_LEGACY_API USL_PlayerLinkedAnimLayer : public USL_BaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	USL_PlayerAnimInstance* GetPlayerAnimInstance() const;
};