#include "AbilitySystem/Abilities/SL_EnemyGameplayAbility.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Utilities/SL_GameplayTags.h"

ASL_EnemyCharacter* USL_EnemyGameplayAbility::GetEnemyCharacterFromActorInfo()
{
	if (!CachedEnemyCharacter.IsValid())
	{
		CachedEnemyCharacter = Cast<ASL_EnemyCharacter>(CurrentActorInfo->AvatarActor);
	}
	
	return CachedEnemyCharacter.IsValid() ? CachedEnemyCharacter.Get() : nullptr;
}

USL_EnemyCombatComponent* USL_EnemyGameplayAbility::GetEnemyCombatComponentFromActorInfo()
{
	if (ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
	{
		return EnemyCharacter->GetEnemyCombatComponent();
	}
	
	return nullptr;
}

FGameplayEffectSpecHandle USL_EnemyGameplayAbility::MakeEnemyDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, const FScalableFloat& InDamageScalableFloat)
{
	check(EffectClass);
	
	FGameplayEffectContextHandle ContextHandle = GetPawnAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	FGameplayEffectSpecHandle EffectSpecHandle = GetPawnAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
	
	EffectSpecHandle.Data->SetSetByCallerMagnitude(
		SL_GameplayTags::Shared_SetByCaller_BaseDamage,
		InDamageScalableFloat.GetValueAtLevel(GetAbilityLevel())
	);
	
	return EffectSpecHandle;
}