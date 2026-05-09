#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_DeathBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interfaces/SL_EnemyDeathInterface.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Enemy_DeathBase::USL_GA_Enemy_DeathBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Shared_Status_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::OwnedTagAdded;
	AbilityTriggers.Add(TriggerData);
}

void USL_GA_Enemy_DeathBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!MontagesToPlay.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, MontagesToPlay.Num() - 1);
		
		if (UAnimMontage* SelectedMontage = MontagesToPlay[RandomIndex])
		{
			UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				SelectedMontage
			);
			PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_DeathBase::OnDeathFinished);
			PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_DeathBase::OnDeathFinished);
			PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_DeathBase::OnDeathFinished);
			PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_DeathBase::OnDeathFinished);
			PlayMontageTask->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	if (DeathSoundGameplayCueTag.IsValid())
	{
		FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		K2_ExecuteGameplayCue(DeathSoundGameplayCueTag, ContextHandle);
	}
}

void USL_GA_Enemy_DeathBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AActor* AvatarActor = ActorInfo->AvatarActor.Get())
	{
		if (AvatarActor->Implements<USL_EnemyDeathInterface>())
		{
			ISL_EnemyDeathInterface::Execute_OnEnemyDied(AvatarActor);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Enemy_DeathBase::OnDeathFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}