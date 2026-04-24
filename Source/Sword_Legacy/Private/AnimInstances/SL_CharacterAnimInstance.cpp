#include "AnimInstances/SL_CharacterAnimInstance.h"
#include "Characters/SL_BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"

void USL_CharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ASL_BaseCharacter>(TryGetPawnOwner());
	
	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void USL_CharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent) return;;
	
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
	bIsSprinting = GroundSpeed > 250.f && bHasAcceleration;
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
}