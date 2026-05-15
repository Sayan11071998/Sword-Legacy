#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "SL_EQSContext_TargetActor.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_EQSContext_TargetActor : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	// ~ Begin UEnvQueryContext Interface
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	// ~ End UEnvQueryContext Interface
};