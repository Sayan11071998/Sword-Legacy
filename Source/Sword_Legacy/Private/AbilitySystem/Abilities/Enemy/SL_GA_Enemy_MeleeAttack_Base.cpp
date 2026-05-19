#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_MeleeAttack_Base.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USL_GA_Enemy_MeleeAttack_Base::USL_GA_Enemy_MeleeAttack_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Enemy_MeleeAttack_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (MontageToPlay)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay
		);

		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}