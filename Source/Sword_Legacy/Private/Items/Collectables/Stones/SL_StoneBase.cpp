#include "Items/Collectables/Stones/SL_StoneBase.h"
#include "Characters/SL_PlayerCharacter.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"
#include "Utilities/SL_GameplayTags.h"

void ASL_StoneBase::Consume(USL_AbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);
	
	UGameplayEffect* EffectCDO = StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	
	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		EffectCDO,
		ApplyLevel,
		AbilitySystemComponent->MakeEffectContext()
	);
	
	BP_OnStoneConsumed();
}

void ASL_StoneBase::OnPickupCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASL_PlayerCharacter* OverlappedPlayerCharacter = Cast<ASL_PlayerCharacter>(OtherActor))
	{
		OverlappedPlayerCharacter->GetCharacterAbilitySystemComponent()->TryActivateAbilityByTag(SL_GameplayTags::Player_Ability_Pickup_Stones);
	}
}