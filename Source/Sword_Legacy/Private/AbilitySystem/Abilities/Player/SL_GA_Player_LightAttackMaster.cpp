#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

USL_GA_Player_LightAttackMaster::USL_GA_Player_LightAttackMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ComboResetTime = 0.3f;
}

void USL_GA_Player_LightAttackMaster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime > ComboResetTime)
	{
		CurrentLightAttackComboCount = 1;
	}
	
	TObjectPtr<UAnimMontage>* MontageToPlay = LightAttackMontagesMap.Find(CurrentLightAttackComboCount);
	
	if (!MontageToPlay || !(*MontageToPlay))
	{
		CurrentLightAttackComboCount = 1;
		MontageToPlay = LightAttackMontagesMap.Find(CurrentLightAttackComboCount);
		
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
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
	
	if (CurrentLightAttackComboCount == LightAttackMontagesMap.Num())
	{
		CurrentLightAttackComboCount = 1;
	}
	else
	{
		CurrentLightAttackComboCount++;
	}
	
	LastAttackTime = CurrentTime;
}

void USL_GA_Player_LightAttackMaster::OnMontageCompleted()
{
	LastAttackTime = GetWorld()->GetTimeSeconds();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}