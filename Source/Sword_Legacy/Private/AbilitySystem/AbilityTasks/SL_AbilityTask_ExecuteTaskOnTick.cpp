#include "AbilitySystem/AbilityTasks/SL_AbilityTask_ExecuteTaskOnTick.h"

USL_AbilityTask_ExecuteTaskOnTick::USL_AbilityTask_ExecuteTaskOnTick()
{
	bTickingTask = true;
}

void USL_AbilityTask_ExecuteTaskOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAbilityTaskTick.Broadcast(DeltaTime);
	}
	else
	{
		EndTask();
	}
}

USL_AbilityTask_ExecuteTaskOnTick* USL_AbilityTask_ExecuteTaskOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	USL_AbilityTask_ExecuteTaskOnTick* Node = NewAbilityTask<USL_AbilityTask_ExecuteTaskOnTick>(OwningAbility);
	
	return Node;
}