#include "AnimNotifies/SL_ANS_AllowRotationOnRootMotion.h"
#include "Characters/SL_PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

void USL_ANS_AllowRotationOnRootMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsGameWorld()) return;

	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(MeshComp->GetOwner());
	if (!PlayerCharacter) return;

	if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
	{
		Movement->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}
}

void USL_ANS_AllowRotationOnRootMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsGameWorld()) return;

	ASL_PlayerCharacter* PlayerCharacter = Cast<ASL_PlayerCharacter>(MeshComp->GetOwner());
	if (!PlayerCharacter) return;

	if (UCharacterMovementComponent* Movement = PlayerCharacter->GetCharacterMovement())
	{
		Movement->bAllowPhysicsRotationDuringAnimRootMotion = false;
	}
}
