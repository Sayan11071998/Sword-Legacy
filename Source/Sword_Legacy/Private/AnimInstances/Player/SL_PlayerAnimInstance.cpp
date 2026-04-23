#include "AnimInstances/Player/SL_PlayerAnimInstance.h"
#include "Characters/SL_PlayerCharacter.h"

void USL_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (OwningCharacter)
	{
		OwningPlayerCharacter = Cast<ASL_PlayerCharacter>(OwningCharacter);
	}
}

void USL_PlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
	if (bHasAcceleration)
	{
		IdleElapsedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = IdleElapsedTime >= EnterRelaxStateThreshold;
	}
}