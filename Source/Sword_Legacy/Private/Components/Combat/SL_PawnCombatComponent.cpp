#include "Components/Combat/SL_PawnCombatComponent.h"

void USL_PawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
	ASL_WeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A tag named %s has already been added as carried weapon"), *InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);
	
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);
	
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