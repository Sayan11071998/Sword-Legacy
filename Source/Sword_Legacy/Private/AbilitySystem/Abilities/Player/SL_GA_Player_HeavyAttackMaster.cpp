#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"

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
	
	if (ComboCountResetTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboCountResetTimerHandle);
	}
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bJumpToFinisher = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);

	if (bJumpToFinisher)
	{
		CurrentHeavyAttackComboCount = HeavyAttackMontagesMap.Num();
	}

	UsedComboCount = CurrentHeavyAttackComboCount;
	
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

	UAbilityTask_WaitGameplayEvent* WaitMeleeHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Shared_Event_MeleeHit,
		nullptr,
		false,
		true
	);
	WaitMeleeHitTask->EventReceived.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMeleeHitEventReceived);
	WaitMeleeHitTask->ReadyForActivation();
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, *MontageToPlay, 1.0f
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_HeavyAttackMaster::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
	
	const int32 TotalMontages = HeavyAttackMontagesMap.Num();

	if (CurrentHeavyAttackComboCount == TotalMontages)
	{
		ResetHeavyAttackComboCount();
	}
	else
	{
		CurrentHeavyAttackComboCount++;
	}
}

void USL_GA_Player_HeavyAttackMaster::OnMeleeHitEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	const float WeaponBaseDamage = CombatComponent->GetPlayerCurrentEquppedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()));

	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		DamageEffectClass,
		WeaponBaseDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Heavy,
		UsedComboCount
	);

	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (TargetActor && DamageSpecHandle.IsValid())
	{
		NativeApplyEffectSpecHandleToTarget(TargetActor, DamageSpecHandle);
	}
}

void USL_GA_Player_HeavyAttackMaster::ResetHeavyAttackComboCount()
{
	CurrentHeavyAttackComboCount = 1;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);
	}
}

void USL_GA_Player_HeavyAttackMaster::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	
	GetWorld()->GetTimerManager().SetTimer(
		ComboCountResetTimerHandle,
		this,
		&USL_GA_Player_HeavyAttackMaster::ResetHeavyAttackComboCount,
		ComboResetTime,
		false
	);
}