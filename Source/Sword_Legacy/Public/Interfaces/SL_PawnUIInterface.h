#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SL_PawnUIInterface.generated.h"

class USL_PawnUIComponent;

UINTERFACE(MinimalAPI)
class USL_PawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

class SWORD_LEGACY_API ISL_PawnUIInterface
{
	GENERATED_BODY()
	
public:
	virtual TObjectPtr<USL_PawnUIComponent> GetPawnUIComponent() const = 0;
};