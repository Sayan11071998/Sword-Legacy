#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightWeaponAbility_Katana.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Utilities/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/UI/SL_PlayerUIComponent.h"

USL_GA_Player_LightWeaponAbility_Katana::USL_GA_Player_LightWeaponAbility_Katana()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

	WeaponDamageLevelMultiplier = 2.0f;
	LevelComboCount = 4;
}

void USL_GA_Player_LightWeaponAbility_Katana::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* WaitMeleeHitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		MeleeHitEventTag,
		nullptr,
		false,
		true
	);
	WaitMeleeHitTask->EventReceived.AddDynamic(this, &USL_GA_Player_LightWeaponAbility_Katana::OnMeleeHitEventReceived);
	WaitMeleeHitTask->ReadyForActivation();

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	float RemainingTime = 0.f;
	float TotalDuration = 0.f;
	GetCooldownTimeRemainingAndDuration(Handle, ActorInfo, RemainingTime, TotalDuration);

	if (USL_PlayerUIComponent* UIComponent = GetPlayerUIComponentFromActorInfo())
	{
		UIComponent->OnAbilityCooldownBegin.Broadcast(AbilityInputTag, TotalDuration, RemainingTime);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		SpecialLightMontage,
		1.0f,
		NAME_None,
		true
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_LightWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_LightWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_LightWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_LightWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
}

void USL_GA_Player_LightWeaponAbility_Katana::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Player_LightWeaponAbility_Katana::OnMeleeHitEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	const float WeaponBaseDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()) * WeaponDamageLevelMultiplier);

	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		DamageEffectClass,
		WeaponBaseDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Light,
		LevelComboCount
	);

	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (!TargetActor || !DamageSpecHandle.IsValid()) return;

	const FActiveGameplayEffectHandle ActiveEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, DamageSpecHandle);

	if (ActiveEffectHandle.WasSuccessfullyApplied())
	{
		if (WeaponHitSoundGameplayCueTag.IsValid())
		{
			FGameplayEffectContextHandle ContextHandle = DamageSpecHandle.Data->GetContext();
			K2_ExecuteGameplayCue(WeaponHitSoundGameplayCueTag, ContextHandle);
		}

		FGameplayEventData HitReactPayload;
		HitReactPayload.Instigator = GetAvatarActorFromActorInfo();
		HitReactPayload.Target = TargetActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			TargetActor,
			SL_GameplayTags::Shared_Event_HitReact,
			HitReactPayload
		);
	}
}