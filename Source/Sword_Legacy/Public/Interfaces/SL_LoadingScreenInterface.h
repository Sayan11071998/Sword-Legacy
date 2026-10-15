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
	
	// virtual void OnLoadingScreenActivated_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenDeactivated();
	
	// virtual void OnLoadingScreenDeactivated_Implementation();
};