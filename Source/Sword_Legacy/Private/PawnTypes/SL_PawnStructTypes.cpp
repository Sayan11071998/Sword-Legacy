#include "PawnTypes/SL_PawnStructTypes.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"

bool FSL_PlayerAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}