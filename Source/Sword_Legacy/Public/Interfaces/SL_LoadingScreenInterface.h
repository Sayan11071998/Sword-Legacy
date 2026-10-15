#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SL_LoadingScreenInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class USL_LoadingScreenInterface : public UInterface
{
	GENERATED_BODY()
};

class SWORD_LEGACY_API ISL_LoadingScreenInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenActivated();

	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenDeactivated();
};