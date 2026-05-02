#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SL_PawnCombatInterface.generated.h"

class USL_PawnCombatComponent;

UINTERFACE(MinimalAPI)
class USL_PawnCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class SWORD_LEGACY_API ISL_PawnCombatInterface
{
	GENERATED_BODY()

public:
	virtual TObjectPtr<USL_PawnCombatComponent> GetPawnCombatComponent() const = 0;
};