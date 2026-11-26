#include "AbilitySystem/Abilities/Player/SL_GA_Player_Parry.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Player_Parry::USL_GA_Player_Parry()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(SL_GameplayTags::Player_Ability_Parry);
	CancelAbilitiesWithTag.AddTag(SL_GameplayTags::Player_Ability_Block);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Player_Event_Parry;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USL_GA_Player_Parry::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::AddGameplayTagToActorIfNone(Avatar, SL_GameplayTags::Player_Status_Parrying);
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, SL_GameplayTags::Player_Status_Blocking);

		if (UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayTagContainer TagsToCancel;
			TagsToCancel.AddTag(SL_GameplayTags::Player_Ability_Block);
			TagsToCancel.AddTag(SL_GameplayTags::Player_Ability_Attack_Light_Katana);
			TagsToCancel.AddTag(SL_GameplayTags::Player_Ability_Attack_Heavy_Katana);

			AbilitySystem->CancelAbilities(&TagsToCancel, nullptr, this);
		}
	}

	if (ActorInfo && ActorInfo->SkeletalMeshComponent.IsValid())
	{
		if (UAnimInstance* AnimInstance = ActorInfo->SkeletalMeshComponent->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.05f);
		}
	}

	if (!ParryMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);

		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ParryMontage
	);
	PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_Parry::OnParryFinished);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_Parry::OnParryFinished);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_Parry::OnParryFinished);
	PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_Parry::OnParryFinished);
	PlayMontageTask->ReadyForActivation();
}

void USL_GA_Player_Parry::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, SL_GameplayTags::Player_Status_Parrying);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USL_GA_Player_Parry::OnParryFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}