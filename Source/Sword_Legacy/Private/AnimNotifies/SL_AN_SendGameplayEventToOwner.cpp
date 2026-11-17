#include "AnimNotifies/SL_AN_SendGameplayEventToOwner.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/World.h"

void USL_AN_SendGameplayEventToOwner::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	const UWorld* World = MeshComp->GetWorld();
	if (!World || !World->IsGameWorld()) return;
	
	if (AActor* OwningActor = MeshComp->GetOwner())
	{
		if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwningActor)) return;

		FGameplayEventData PayLoad;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningActor, EventTag, PayLoad);
	}
}