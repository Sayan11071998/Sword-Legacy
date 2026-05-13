#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SL_AIController.generated.h"

UCLASS()
class SWORD_LEGACY_API ASL_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASL_AIController(const FObjectInitializer& ObjectInitializer);
};