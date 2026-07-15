#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SL_UISubsystem.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_UISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static TObjectPtr<USL_UISubsystem> Get(const TObjectPtr<UObject> WorldContextObject);
};