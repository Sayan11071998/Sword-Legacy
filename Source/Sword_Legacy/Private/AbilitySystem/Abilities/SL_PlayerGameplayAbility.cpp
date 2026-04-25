#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "Characters/SL_PlayerCharacter.h"

ASL_PlayerCharacter* USL_PlayerGameplayAbility::GetPlayerCharacterFromActorInfo()
{
	if (!CachedPlayerCharacter.IsValid())
	{
		CachedPlayerCharacter = Cast<ASL_PlayerCharacter>(CurrentActorInfo->AvatarActor);
	}
	
	return CachedPlayerCharacter.IsValid() ? CachedPlayerCharacter.Get() : nullptr;
}