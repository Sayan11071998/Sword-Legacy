#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/SL_BaseAnimInstance.h"
#include "SL_CharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class ASL_BaseCharacter;

UCLASS()
class SWORD_LEGACY_API USL_CharacterAnimInstance : public USL_BaseAnimInstance
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	// ~ End UAnimInstance interface
	
protected:
	UPROPERTY()
	TObjectPtr<ASL_BaseCharacter> OwningCharacter;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwningMovementComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float GroundSpeed;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bHasAcceleration;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bIsSprinting;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float LocomotionDirection;
};