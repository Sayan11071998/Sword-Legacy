#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SL_EnemyDeathInterface.generated.h"

class UNiagaraSystem;

UINTERFACE(MinimalAPI)
class USL_EnemyDeathInterface : public UInterface
{
	GENERATED_BODY()
};

class SWORD_LEGACY_API ISL_EnemyDeathInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Enemy")
	void OnEnemyDied(const TSoftObjectPtr<UNiagaraSystem>& InSoftNiagaraSystem);
};