#include "PawnTypes/SL_PawnStructTypes.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"

bool FSL_PlayerAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}