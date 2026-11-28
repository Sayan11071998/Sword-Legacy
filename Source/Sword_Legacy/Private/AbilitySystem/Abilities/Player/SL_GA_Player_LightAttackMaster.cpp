#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

USL_GA_Player_LightAttackMaster::USL_GA_Player_LightAttackMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_LightAttackMaster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PrepareComboAttack();

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bJumpToFinisher = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);

	int32 ComboCount = GetComboCount();
	UAnimMontage* ChosenMontage = nullptr;

	if (bJumpToFinisher && ParryCounterMontage)
	{
		ChosenMontage = ParryCounterMontage;
		UsedComboCount = ParryCounterDamageCount;

		ResetComboCount();
	}
	else
	{
		if (bJumpToFinisher)
		{
			ComboCount = LightAttackMontagesMap.Num();
		}

		UsedComboCount = ComboCount;

		if (TObjectPtr<UAnimMontage>* FoundMontage = LightAttackMontagesMap.Find(ComboCount))
		{
			ChosenMontage = FoundMontage->Get();
		}
	}

	UAnimMontage* MontageToPlay = ChosenMontage;
	const bool bPlayedParryCounter = bJumpToFinisher && ParryCounterMontage;

	if (!MontageToPlay)
	{
		ComboCount = 1;
		if (TObjectPtr<UAnimMontage>* FoundMontage = LightAttackMontagesMap.Find(ComboCount))
		{
			MontageToPlay = FoundMontage->Get();
		}

		if (!MontageToPlay)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	SetupMeleeMotionWarp(LightAttackWarpTargetName, LightAttackWarpDistance, LightAttackStopDistance, LightAttackWarpHalfAngle);
	
	UAbilityTask_WaitGameplayEvent* WaitMeleeHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Shared_Event_MeleeHit,
		nullptr,
		false,
		true
	);
	WaitMeleeHitTask->EventReceived.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMeleeHitEventReceived);
	WaitMeleeHitTask->ReadyForActivation();
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		MontageToPlay,
		1.0f
	);

	BindComboMontage(PlayMontageTask, MontageToPlay);

	PlayMontageTask->ReadyForActivation();

	if (!bPlayedParryCounter)
	{
		const int32 TotalMontages = LightAttackMontagesMap.Num();

		if (ComboCount == TotalMontages)
		{
			ResetComboCount();
		}
		else
		{
			SetComboCount(ComboCount + 1);
		}
	}

	if (AvatarActor && USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_Rage_Active))
	{
		WhileRageActive();
	}
}

void USL_GA_Player_LightAttackMaster::OnMeleeHitEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	const float WeaponBaseDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()));
	
	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		DamageEffectClass,
		WeaponBaseDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Light,
		UsedComboCount
	);

	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (!TargetActor || !DamageSpecHandle.IsValid()) return;

	if (USL_FunctionLibrary::NotifyEnemyBlockedAttack(GetAvatarActorFromActorInfo(), TargetActor)) return;

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

void USL_GA_Player_LightAttackMaster::WhileRageActive_Implementation() { }