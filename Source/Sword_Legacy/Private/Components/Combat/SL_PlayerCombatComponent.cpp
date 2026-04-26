#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"

ASL_PlayerWeapon* USL_PlayerCombatComponent::GetPlayerCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<ASL_PlayerWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}