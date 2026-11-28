#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_ComboAttack.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Enemy_ComboAttack::USL_GA_Enemy_ComboAttack()
{
	AbilityTags.AddTag(SL_GameplayTags::Enemy_Ability_Combo);
	ActivationBlockedTags.AddTag(SL_GameplayTags::Enemy_Cooldown_Combo);
}

void USL_GA_Enemy_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bCooldownStarted = false;

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USL_GA_Enemy_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	const bool bStartCooldown = MontageToPlay && ComboCooldown > 0.f && !bCooldownStarted;

	if (bStartCooldown)
	{
		bCooldownStarted = true;

		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			USL_FunctionLibrary::AddGameplayTagToActorIfNone(Avatar, SL_GameplayTags::Enemy_Cooldown_Combo);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bStartCooldown)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ComboCooldownTimer, this, &USL_GA_Enemy_ComboAttack::ClearComboCooldown, ComboCooldown, false);
		}
	}
}

void USL_GA_Enemy_ComboAttack::ClearComboCooldown()
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, SL_GameplayTags::Enemy_Cooldown_Combo);
	}
}