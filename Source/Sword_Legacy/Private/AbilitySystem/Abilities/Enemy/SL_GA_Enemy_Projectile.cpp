#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_Projectile.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USL_GA_Enemy_Projectile::USL_GA_Enemy_Projectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Enemy_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_Projectile::OnMontageFinished);
		
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void USL_GA_Enemy_Projectile::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}