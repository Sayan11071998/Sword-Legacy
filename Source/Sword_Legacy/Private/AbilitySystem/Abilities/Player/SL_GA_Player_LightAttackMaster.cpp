#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Utilities/SL_FunctionLibrary.h"

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

	int32 ComboIndexToPlay = CurrentLightAttackComboCount;
	TObjectPtr<UAnimMontage>* MontageToPlay = LightAttackMontagesMap.Find(ComboIndexToPlay);

	if (!MontageToPlay || !(*MontageToPlay))
	{
		CurrentLightAttackComboCount = 1;
		MontageToPlay = LightAttackMontagesMap.Find(1);

		if (!MontageToPlay || !(*MontageToPlay))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}
	
	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		const int32 TotalLightAttacks = LightAttackMontagesMap.Num();
		
		if (CurrentLightAttackComboCount == TotalLightAttacks - 1 && TotalLightAttacks >= 2)
		{
			USL_FunctionLibrary::AddGameplayToActorIfNone(
				ActorInfo->AvatarActor.Get(), 
				SL_GameplayTags::Player_Status_JumpToFinisher
			);
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