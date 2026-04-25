#include "AnimNotifies/SL_AN_SendGameplayEventToOwner.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"

void USL_AN_SendGameplayEventToOwner::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp)
	{
		if (AActor* OwningActor = MeshComp->GetOwner())
		{
			FGameplayEventData PayLoad;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningActor, EventTag, PayLoad);
		}
	}
}