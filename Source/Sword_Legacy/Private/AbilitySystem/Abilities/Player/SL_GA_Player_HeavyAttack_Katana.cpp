#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttack_Katana.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_RageHeavyBlink.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Player_HeavyAttack_Katana::USL_GA_Player_HeavyAttack_Katana()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USL_GA_Player_HeavyAttack_Katana::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AActor* AvatarActor = ActorInfo ? ActorInfo->AvatarActor.Get() : nullptr;

	const bool bRageActive = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_Rage_Active);
	
	const bool bParryFollowUp = AvatarActor &&
		USL_FunctionLibrary::NativeDoesActorHaveTag(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);

	if (bRageActive && !bParryFollowUp)
	{
		UGameplayAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
		PrepareComboAttack();
		ResetComboCount();

		USL_AbilitySystemComponent* AbilitySystem = ActorInfo
			? Cast<USL_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get())
			: nullptr;

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

		if (AbilitySystem)
		{
			TryStartRageHeavyBlink(ActorInfo);
		}

		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USL_GA_Player_HeavyAttack_Katana::TryStartRageHeavyBlink(const FGameplayAbilityActorInfo* ActorInfo) const
{
	USL_AbilitySystemComponent* AbilitySystem = ActorInfo
		? Cast<USL_AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get())
		: nullptr;
	
	if (!AbilitySystem) return false;

	if (AbilitySystem->TryActivateAbilityByTag(SL_GameplayTags::Player_Ability_RageHeavyBlink))
	{
		return true;
	}

	for (const FGameplayAbilitySpec& Spec : AbilitySystem->GetActivatableAbilities())
	{
		if (!Spec.Ability || !Spec.Ability->GetClass()->IsChildOf(USL_GA_Player_RageHeavyBlink::StaticClass())) continue;
		
		if (Spec.IsActive()) return true;
		
		return AbilitySystem->TryActivateAbility(Spec.Handle);
	}

	return false;
}