#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Components/Combat/SL_PawnCombatComponent.h"

void USL_GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	
	if (AbilityActivationPolicy == ESL_AbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void USL_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (AbilityActivationPolicy == ESL_AbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

USL_PawnCombatComponent* USL_GameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<USL_PawnCombatComponent>();
}

USL_AbilitySystemComponent* USL_GameplayAbility::GetPawnAbilitySystemComponentFromActorInfo() const
{
	return Cast<USL_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}