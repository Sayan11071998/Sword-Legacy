#include "Components/Combat/SL_PawnCombatComponent.h"
#include "Items/Weapons/SL_WeaponBase.h"
#include "Components/BoxComponent.h"

void USL_PawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
	ASL_WeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A tag named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);
	
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);
	
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &USL_PawnCombatComponent::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &USL_PawnCombatComponent::OnWeaponPulledFromTargetActor);
	
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
}

ASL_WeaponBase* USL_PawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		if (ASL_WeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		}
	}
	
	return nullptr;
}

ASL_WeaponBase* USL_PawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid()) return nullptr;
	
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void USL_PawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, ESL_ToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == ESL_ToggleDamageType::CurrentEquippedWeapon)
	{
		ASL_WeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
		check(WeaponToToggle);
		
		if (bShouldEnable)
		{
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			OverlappedActors.Empty();
		}
	}
}

void USL_PawnCombatComponent::OnHitTargetActor(AActor* HitActor) { }
void USL_PawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor) { }