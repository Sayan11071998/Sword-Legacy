#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

USL_GA_Player_HeavyAttackMaster::USL_GA_Player_HeavyAttackMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_HeavyAttackMaster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PrepareComboAttack();
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bJumpToFinisher = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);

	int32 ComboCount = GetComboCount();

	if (bJumpToFinisher)
	{
		ComboCount = HeavyAttackMontagesMap.Num();
	}

	UsedComboCount = ComboCount;
	
	TObjectPtr<UAnimMontage>* MontageToPlay = HeavyAttackMontagesMap.Find(ComboCount);

	if (!MontageToPlay || !(*MontageToPlay))
	{
		ComboCount = 1;
		MontageToPlay = HeavyAttackMontagesMap.Find(ComboCount);

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
	
	SetupMeleeMotionWarp(HeavyAttackWarpTargetName, HeavyAttackWarpDistance, HeavyAttackStopDistance, HeavyAttackWarpHalfAngle);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, *MontageToPlay, 1.0f
	);
	BindComboMontage(PlayMontageTask, *MontageToPlay);
	PlayMontageTask->ReadyForActivation();
	
	const int32 TotalMontages = HeavyAttackMontagesMap.Num();

	if (ComboCount == TotalMontages)
	{
		ResetComboCount();
	}
	else
	{
		SetComboCount(ComboCount + 1);
	}

	if (AvatarActor && USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_Rage_Active))
	{
		WhileRageActive();
	}
}

void USL_GA_Player_HeavyAttackMaster::OnMeleeHitEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	const float WeaponBaseDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()));

	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		DamageEffectClass,
		WeaponBaseDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Heavy,
		UsedComboCount
	);

	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());

	if (USL_FunctionLibrary::NotifyEnemyBlockedAttack(GetAvatarActorFromActorInfo(), TargetActor)) return;

	if (TargetActor && DamageSpecHandle.IsValid())
	{
		if (WeaponHitSoundGameplayCueTag.IsValid())
		{
			FGameplayEffectContextHandle ContextHandle = DamageSpecHandle.Data->GetContext();
			K2_ExecuteGameplayCue(WeaponHitSoundGameplayCueTag, ContextHandle);
		}
		
		const FActiveGameplayEffectHandle ActiveEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, DamageSpecHandle);

		if (ActiveEffectHandle.WasSuccessfullyApplied())
		{
			FGameplayEventData HitReactPayload;
			HitReactPayload.Instigator = GetAvatarActorFromActorInfo();
			HitReactPayload.Target = TargetActor;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				TargetActor,
				SL_GameplayTags::Shared_Event_HitReact,
				HitReactPayload
			);

			if (GainRageEffectClass)
			{
				if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
				{
					FGameplayEffectSpecHandle RageSpecHandle = MakeOutgoingGameplayEffectSpec(GainRageEffectClass, GetAbilityLevel());
					if (RageSpecHandle.IsValid())
					{
						ASC->ApplyGameplayEffectSpecToSelf(*RageSpecHandle.Data.Get());
					}
				}
			}
		}
	}
}

void USL_GA_Player_HeavyAttackMaster::WhileRageActive_Implementation() { }