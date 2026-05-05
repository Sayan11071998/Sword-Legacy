#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_HitReactBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USL_GA_Enemy_HitReactBase::USL_GA_Enemy_HitReactBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Enemy_HitReactBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!MontagesToPlay.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, MontagesToPlay.Num() - 1);
		UAnimMontage* SelectedMontage = MontagesToPlay[RandomIndex];
		
		if (SelectedMontage)
		{
			UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				SelectedMontage
			);
			PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
			PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
			PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
			PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_HitReactBase::OnHitReactFinished);
			
			PlayMontageTask->ReadyForActivation();
			return;
		}
		
		FTimerHandle DummyTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DummyTimerHandle, [this]()
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}, 0.2f, false);
	}
}

void USL_GA_Enemy_HitReactBase::OnHitReactFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}