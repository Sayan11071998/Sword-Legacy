#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USL_GA_Player_HeavyAttackMaster::USL_GA_Player_HeavyAttackMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ComboResetTime = 0.3f;
}

void USL_GA_Player_HeavyAttackMaster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime > ComboResetTime)
	{
		CurrentHeavyAttackComboCount = 1;
	}
	
	TObjectPtr<UAnimMontage>* MontageToPlay = HeavyAttackMontagesMap.Find(CurrentHeavyAttackComboCount);
	
	if (!MontageToPlay || !(*MontageToPlay))
	{
		CurrentHeavyAttackComboCount = 1;
		MontageToPlay = HeavyAttackMontagesMap.Find(CurrentHeavyAttackComboCount);
		
		if (!MontageToPlay || !(*MontageToPlay))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		*MontageToPlay,
		1.0f
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
	
	if (CurrentHeavyAttackComboCount == HeavyAttackMontagesMap.Num())
	{
		CurrentHeavyAttackComboCount = 1;
	}
	else
	{
		CurrentHeavyAttackComboCount++;
	}
	
	LastAttackTime = CurrentTime;
}

void USL_GA_Player_HeavyAttackMaster::OnMontageCompleted()
{
	LastAttackTime = GetWorld()->GetTimeSeconds();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}