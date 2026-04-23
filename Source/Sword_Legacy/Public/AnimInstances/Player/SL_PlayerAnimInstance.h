#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/SL_CharacterAnimInstance.h"
#include "SL_PlayerAnimInstance.generated.h"

class ASL_PlayerCharacter;

UCLASS()
class SWORD_LEGACY_API USL_PlayerAnimInstance : public USL_CharacterAnimInstance
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	// ~ End UAnimInstance interface
	
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|References")
	TObjectPtr<ASL_PlayerCharacter> OwningPlayerCharacter;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	bool bShouldEnterRelaxState;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|LocomotionData")
	float EnterRelaxStateThreshold = 5.f;
	
	float IdleElapsedTime;
};