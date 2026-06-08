#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_MeleeAttack_Base.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Utilities/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

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
	
	bool bIsUnblockable = false;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		bIsUnblockable = ASC->HasMatchingGameplayTag(SL_GameplayTags::Enemy_Status_Unblockable);
	}

	if (bIsUnblockable)
	{
		FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		K2_ExecuteGameplayCue(SL_GameplayTags::GameplayCue_Effects_UnblockableWarning, ContextHandle);

		UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, 0.2f);
		if (WaitDelayTask)
		{
			WaitDelayTask->OnFinish.AddDynamic(this, &USL_GA_Enemy_MeleeAttack_Base::PlayMeleeAttackMontage);
			WaitDelayTask->ReadyForActivation();
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
	else
	{
		PlayMeleeAttackMontage();
	}
}

void USL_GA_Enemy_MeleeAttack_Base::PlayMeleeAttackMontage()
{
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
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				TargetActor,
				SL_GameplayTags::Shared_Event_HitReact,
				Payload
			);
		}
	}
}

void USL_GA_Enemy_MeleeAttack_Base::OnMeleeAttackFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}