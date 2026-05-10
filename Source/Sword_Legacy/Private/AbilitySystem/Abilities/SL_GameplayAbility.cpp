#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Components/Combat/SL_PawnCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

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

FActiveGameplayEffectHandle USL_GameplayAbility::NativeApplyEffectSpecHandleToTarget(TObjectPtr<AActor> TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	
	check(TargetASC && InSpecHandle.IsValid());
	
	return GetPawnAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
}

USL_PawnCombatComponent* USL_GameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<USL_PawnCombatComponent>();
}

USL_AbilitySystemComponent* USL_GameplayAbility::GetPawnAbilitySystemComponentFromActorInfo() const
{
	return Cast<USL_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle USL_GameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, ESL_SuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? ESL_SuccessType::Successful : ESL_SuccessType::Failed;

	return ActiveGameplayEffectHandle;
}