#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Utilities/SL_GameplayTags.h"

void USL_PlayerCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor)) return;
	
	OverlappedActors.AddUnique(HitActor);
	
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		SL_GameplayTags::Shared_Event_MeleeHit,
		Data
	);
}

void USL_PlayerCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}

ASL_PlayerWeapon* USL_PlayerCombatComponent::GetPlayerCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<ASL_PlayerWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}