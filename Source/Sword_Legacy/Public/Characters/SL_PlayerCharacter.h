#pragma once

#include "CoreMinimal.h"
#include "Characters/SL_BaseCharacter.h"
#include "SL_PlayerCharacter.generated.h"

struct FInputActionValue;
class USL_DataAsset_InputConfig;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SWORD_LEGACY_API ASL_PlayerCharacter : public ASL_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ASL_PlayerCharacter();
	
	// ~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	// ~ End ACharacter interface
	
private:
#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
#pragma endregion
	
#pragma region Input
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_DataAsset_InputConfig> InputConfigDataAsset;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	
#pragma endregion
};