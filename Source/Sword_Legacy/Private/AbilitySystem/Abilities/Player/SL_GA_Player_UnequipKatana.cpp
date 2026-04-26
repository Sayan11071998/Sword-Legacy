#include "AbilitySystem/Abilities/Player/SL_GA_Player_UnequipKatana.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Controllers/SL_PlayerController.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AnimInstances/Player/SL_PlayerLinkedAnimLayer.h"

void USL_GA_Player_UnequipKatana::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		UnequipMontage,
		1.f
	);
	
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_UnequipKatana::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_UnequipKatana::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_UnequipKatana::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_UnequipKatana::OnMontageCompleted);
	PlayMontageTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		UnequipEventTag,
		nullptr,
		true,
		true
	);
	
	WaitEventTask->EventReceived.AddDynamic(this, &USL_GA_Player_UnequipKatana::OnUnequipEventReceived);
	WaitEventTask->ReadyForActivation();
}

void USL_GA_Player_UnequipKatana::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Player_UnequipKatana::OnUnequipEventReceived(FGameplayEventData Payload)
{
	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;
	
	ASL_PlayerWeapon* Weapon = CombatComponent->GetPlayerCarriedWeaponByTag(WeaponTagToUnequip);
	if (!Weapon) return;
	
	if (USkeletalMeshComponent* SkeletalMesh = CurrentActorInfo->SkeletalMeshComponent.Get())
	{
		const FAttachmentTransformRules AttachRules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			true
		);
		
		Weapon->AttachToComponent(SkeletalMesh, AttachRules, UnequipSocketName);
		HandleUnequipWeapon(Weapon);
	}
	
	CombatComponent->CurrentEquippedWeaponTag = FGameplayTag::EmptyTag;
}

void USL_GA_Player_UnequipKatana::HandleUnequipWeapon(ASL_PlayerWeapon* InWeapon)
{
	if (!InWeapon) return;
	
	const FSL_PlayerWeaponData& WeaponData = InWeapon->PlayerWeaponData;

	if (WeaponData.WeaponAnimLayerToLink)
	{
		if (USkeletalMeshComponent* SkeletalMesh = GetActorInfo().SkeletalMeshComponent.Get())
		{
			if (UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance())
			{
				AnimInstance->UnlinkAnimClassLayers(WeaponData.WeaponAnimLayerToLink);
			}
		}
	}

	if (WeaponData.WeaponInputMappingContext)
	{
		if (ASL_PlayerController* PlayerController = GetPlayerControllerFromActorInfo())
		{
			if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					Subsystem->RemoveMappingContext(WeaponData.WeaponInputMappingContext);
				}
			}
		}
	}

	if (USL_AbilitySystemComponent* ASCToRemove = GetPawnAbilitySystemComponentFromActorInfo())
	{
		TArray<FGameplayAbilitySpecHandle> HandlesToRemove = InWeapon->GetGrantedAbilitySpecHandles();
		ASCToRemove->RemoveGrantedPlayerWeaponAbilities(HandlesToRemove);
	}
}