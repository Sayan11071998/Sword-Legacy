#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyWeaponAbility_Katana.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Utilities/SL_GameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/UI/SL_PlayerUIComponent.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

USL_GA_Player_HeavyWeaponAbility_Katana::USL_GA_Player_HeavyWeaponAbility_Katana()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USL_GA_Player_HeavyWeaponAbility_Katana::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilityTask_WaitGameplayEvent* WaitAOETask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		AOEEventTag,
		nullptr,
		false,
		true
	);
	WaitAOETask->EventReceived.AddDynamic(this, &USL_GA_Player_HeavyWeaponAbility_Katana::OnAOEEventReceived);
	WaitAOETask->ReadyForActivation();

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
		SpecialHeavyMontage,
		1.0f,
		NAME_None,
		true
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_HeavyWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_HeavyWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_HeavyWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_HeavyWeaponAbility_Katana::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
}

void USL_GA_Player_HeavyWeaponAbility_Katana::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Player_HeavyWeaponAbility_Katana::OnAOEEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	const float WeaponBaseDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()) * WeaponDamageLevelMultiplier);

	const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
		DamageEffectClass,
		WeaponBaseDamage,
		SL_GameplayTags::Player_SetByCaller_AttackType_Heavy,
		UsedComboCount
	);

	if (!DamageSpecHandle.IsValid()) return;

	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (!PlayerCharacter) return;

	const FVector StartLocation = PlayerCharacter->GetActorLocation();
	const FVector ForwardVector = PlayerCharacter->GetActorForwardVector();
	const FVector EndLocation = StartLocation + (ForwardVector * BoxTraceDistance);

	TArray<FHitResult> OutHits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(PlayerCharacter);

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		PlayerCharacter,
		StartLocation,
		EndLocation,
		BoxTraceHalfSize,
		ForwardVector.ToOrientationRotator(),
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Persistent,
		OutHits,
		true
	);

	ApplyGameplayEffectSpecHandleToHitResults(DamageSpecHandle, OutHits);
}