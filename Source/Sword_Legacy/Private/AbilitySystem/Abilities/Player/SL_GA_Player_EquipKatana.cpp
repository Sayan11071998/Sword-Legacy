#include "AbilitySystem/Abilities/Player/SL_GA_Player_EquipKatana.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Items/Weapons/SL_WeaponBase.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"

void USL_GA_Player_EquipKatana::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		EquipMontage,
		1.f
	);
	
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_EquipKatana::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_EquipKatana::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_EquipKatana::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_EquipKatana::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		EquipEventTag,
		nullptr,
		true,
		true	
	);
	
	WaitEventTask->EventReceived.AddDynamic(this, &USL_GA_Player_EquipKatana::OnEquipEventReceived);
	WaitEventTask->ReadyForActivation();
}

void USL_GA_Player_EquipKatana::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Player_EquipKatana::OnEquipEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	ASL_WeaponBase* Weapon = CombatComponent->GetCharacterCarriedWeaponByTag(WeaponTagToEquip);
	if (!Weapon) return;

	Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (USkeletalMeshComponent* SkeletalMesh = CurrentActorInfo->SkeletalMeshComponent.Get())
	{
		const FAttachmentTransformRules AttachRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		);
		Weapon->AttachToComponent(SkeletalMesh, AttachRules, EquipSocketName);
	}

	CombatComponent->CurrentEquippedWeaponTag = WeaponTagToEquip;
}