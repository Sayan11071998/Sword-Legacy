#pragma once

#include "CoreMinimal.h"
#include "Characters/SL_BaseCharacter.h"
#include "SL_PlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SWORD_LEGACY_API ASL_PlayerCharacter : public ASL_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ASL_PlayerCharacter();
	
	virtual void BeginPlay() override;
	
private:
#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
#pragma endregion
};