#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SL_AbilityTask_ExecuteTaskOnTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskTickDelegate, float, DeltaTime);

UCLASS()
class SWORD_LEGACY_API USL_AbilityTask_ExecuteTaskOnTick : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	USL_AbilityTask_ExecuteTaskOnTick();
	
	// ~ Begin UGameplayTask Interface
	virtual void TickTask(float DeltaTime) override;
	// ~ End UGameplayTask Interface
	
	UFUNCTION(BlueprintCallable, Category = "Pawn|AbilityTasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static USL_AbilityTask_ExecuteTaskOnTick* ExecuteTaskOnTick(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FOnAbilityTaskTickDelegate OnAbilityTaskTick;
};