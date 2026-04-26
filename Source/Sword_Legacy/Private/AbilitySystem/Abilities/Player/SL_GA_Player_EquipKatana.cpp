#include "AbilitySystem/Abilities/Player/SL_GA_Player_EquipKatana.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "AnimInstances/Player/SL_PlayerLinkedAnimLayer.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Controllers/SL_PlayerController.h"

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

	ASL_PlayerWeapon* Weapon = CombatComponent->GetPlayerCarriedWeaponByTag(WeaponTagToEquip);
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
		
		HandleEquipWeapon(Weapon);
	}

	CombatComponent->CurrentEquippedWeaponTag = WeaponTagToEquip;
}

void USL_GA_Player_EquipKatana::HandleEquipWeapon(ASL_PlayerWeapon* InWeapon)
{
	if (!InWeapon) return;

	const FSL_PlayerWeaponData& WeaponData = InWeapon->PlayerWeaponData;
	
	if (WeaponData.WeaponAnimLayerToLink)
	{
		if (USkeletalMeshComponent* SkeletalMesh = GetActorInfo().SkeletalMeshComponent.Get())
		{
			if (UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance())
			{
				AnimInstance->LinkAnimClassLayers(WeaponData.WeaponAnimLayerToLink);
			}
		}
	}
	
	if (USL_AbilitySystemComponent* ASCToGive = GetPawnAbilitySystemComponentFromActorInfo())
	{
		TArray<FGameplayAbilitySpecHandle> OutHandles;
		ASCToGive->GrantPlayerWeaponAbilities(WeaponData.DefaultWeaponAbilities, GetAbilityLevel(), OutHandles);
	}
	
	if (WeaponData.WeaponInputMappingContext)
	{
		if (ASL_PlayerController* PC = GetPlayerControllerFromActorInfo())
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					Subsystem->AddMappingContext(WeaponData.WeaponInputMappingContext, 1);
				}
			}
		}
	}
}