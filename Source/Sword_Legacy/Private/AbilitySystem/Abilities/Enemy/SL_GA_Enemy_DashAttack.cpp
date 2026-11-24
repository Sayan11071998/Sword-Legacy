#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_DashAttack.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "TimerManager.h"
#include "Utilities/SL_FunctionLibrary.h"

USL_GA_Enemy_DashAttack::USL_GA_Enemy_DashAttack()
{
	AbilityTags.AddTag(SL_GameplayTags::Enemy_Ability_DashAttack);
	ActivationBlockedTags.AddTag(SL_GameplayTags::Enemy_Cooldown_DashAttack);

	AttackWarpDistance = 900.f;
	AttackStopDistance = 140.f;
	AttackWarpHalfAngle = 180.f;
}

void USL_GA_Enemy_DashAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	bCooldownStarted = false;

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USL_GA_Enemy_DashAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	const bool bStartCooldown = MontageToPlay && DashCooldown > 0.f && !bCooldownStarted;

	if (bStartCooldown)
	{
		bCooldownStarted = true;

		if (AActor* Avatar = GetAvatarActorFromActorInfo())
		{
			USL_FunctionLibrary::AddGameplayTagToActorIfNone(Avatar, SL_GameplayTags::Enemy_Cooldown_DashAttack);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bStartCooldown)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(DashCooldownTimer, this, &USL_GA_Enemy_DashAttack::ClearDashCooldown, DashCooldown, false);
		}
	}
}

void USL_GA_Enemy_DashAttack::ClearDashCooldown()
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(Avatar, SL_GameplayTags::Enemy_Cooldown_DashAttack);
	}
}