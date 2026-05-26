#include "AbilitySystem/Abilities/Player/SL_GA_Player_Block.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USL_GA_Player_Block::USL_GA_Player_Block()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (BlockMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			BlockMontage
		);
		
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Block::OnBlockFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Block::OnBlockFinished);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Player_Block::OnBlockFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}