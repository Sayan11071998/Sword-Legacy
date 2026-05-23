#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_MeleeAttack_Base.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Enemy_MeleeAttack_Base::USL_GA_Enemy_MeleeAttack_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Enemy_MeleeAttack_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Shared_Event_MeleeHit,
		nullptr,
		false,
		true
	);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::HandleApplyDamage);
	WaitGameplayEventTask->ReadyForActivation();
	
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

void USL_GA_Enemy_MeleeAttack_Base::HandleApplyDamage(FGameplayEventData Payload)
{
	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (!TargetActor) return;
	
	if (DamageEffectClass)
	{
		const FGameplayEffectSpecHandle DamageSpecHandle = MakeEnemyDamageEffectSpecHandle(DamageEffectClass, InDamageScalableFloat);
		if (DamageSpecHandle.IsValid())
		{
			NativeApplyEffectSpecHandleToTarget(TargetActor, DamageSpecHandle);

			if (WeaponHitSoundGameplayCueTag.IsValid())
			{
				FGameplayEffectContextHandle ContextHandle = DamageSpecHandle.Data->GetContext();
				K2_ExecuteGameplayCue(WeaponHitSoundGameplayCueTag, ContextHandle);
			}
		}
	}
}

void USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}