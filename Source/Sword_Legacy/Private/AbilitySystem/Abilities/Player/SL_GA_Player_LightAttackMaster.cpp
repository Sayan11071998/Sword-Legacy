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

	bComboWindowOpen = false;
	bComboInputBuffered = false;
	bComboChainRequested = false;
	bMontageEndHandled = false;
	PlayingMontage = nullptr;

	UnbindActiveMontageTask();

	if (ComboCountResetTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboCountResetTimerHandle);
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const bool bJumpToFinisher = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);

	if (bJumpToFinisher)
	{
		CurrentLightAttackComboCount = LightAttackMontagesMap.Num();
	}

	UsedComboCount = CurrentLightAttackComboCount;

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
		*MontageToPlay,
		1.0f
	);

	PlayingMontage = *MontageToPlay;
	ActiveMontageTask = PlayMontageTask;

	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
	
	const int32 TotalMontages = LightAttackMontagesMap.Num();

	if (CurrentLightAttackComboCount == TotalMontages)
	{
		ResetLightAttackComboCount();
	}
	else
	{
		CurrentLightAttackComboCount++;
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

void USL_GA_Player_LightAttackMaster::ResetLightAttackComboCount()
{
	CurrentLightAttackComboCount = 1;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);
	}
}

void USL_GA_Player_LightAttackMaster::ReceiveComboInput()
{
	if (!IsActive() || bComboChainRequested) return;

	bComboInputBuffered = true;

	if (bComboWindowOpen)
	{
		RequestComboChain();
	}
}

void USL_GA_Player_LightAttackMaster::OpenComboWindow(UAnimMontage* SourceMontage)
{
	if (!IsActive() || bComboChainRequested || SourceMontage != PlayingMontage) return;

	bComboWindowOpen = true;

	if (bComboInputBuffered)
	{
		RequestComboChain();
	}
}

void USL_GA_Player_LightAttackMaster::CloseComboWindow(UAnimMontage* SourceMontage)
{
	if (SourceMontage != PlayingMontage) return;

	bComboWindowOpen = false;
}

void USL_GA_Player_LightAttackMaster::RequestComboChain()
{
	if (!IsActive() || bComboChainRequested) return;

	bComboChainRequested = true;
	bComboWindowOpen = false;
	bComboInputBuffered = false;
	bMontageEndHandled = true;

	UnbindActiveMontageTask();

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	UWorld* World = GetWorld();

	const FGameplayAbilitySpecHandle Handle = CurrentSpecHandle;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	if (AbilitySystemComponent && AbilitySystemComponent->TryActivateAbility(Handle)) return;

	StartComboResetTimer(World);
}

void USL_GA_Player_LightAttackMaster::UnbindActiveMontageTask()
{
	if (!ActiveMontageTask) return;

	ActiveMontageTask->OnCompleted.RemoveDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	ActiveMontageTask->OnBlendOut.RemoveDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	ActiveMontageTask->OnInterrupted.RemoveDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	ActiveMontageTask->OnCancelled.RemoveDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
	ActiveMontageTask = nullptr;
}

void USL_GA_Player_LightAttackMaster::StartComboResetTimer(UWorld* World)
{
	if (!World) return;

	World->GetTimerManager().SetTimer(
		ComboCountResetTimerHandle,
		this,
		&USL_GA_Player_LightAttackMaster::ResetLightAttackComboCount,
		ComboResetTime,
		false
	);
}

void USL_GA_Player_LightAttackMaster::OnMontageCompleted()
{
	if (bMontageEndHandled || bComboChainRequested) return;

	bMontageEndHandled = true;

	if (bComboInputBuffered)
	{
		RequestComboChain();
		return;
	}

	UWorld* World = GetWorld();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	StartComboResetTimer(World);
}

void USL_GA_Player_LightAttackMaster::WhileRageActive_Implementation() { }