#include "AnimNotifies/SL_ANS_ToggleWeaponCollision.h"
#include "Components/Combat/SL_PawnCombatComponent.h"
#include "Utilities/SL_FunctionLibrary.h"

void USL_ANS_ToggleWeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (USL_PawnCombatComponent* CombatComponent = USL_FunctionLibrary::NativeGetPawnCombatComponentFromActor(MeshComp->GetOwner()))
	{
		CombatComponent->ToggleWeaponCollision(true, ToggleDamageType);
	}
}

void USL_ANS_ToggleWeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (USL_PawnCombatComponent* CombatComponent = USL_FunctionLibrary::NativeGetPawnCombatComponentFromActor(MeshComp->GetOwner()))
	{
		CombatComponent->ToggleWeaponCollision(false, ToggleDamageType);
	}
}