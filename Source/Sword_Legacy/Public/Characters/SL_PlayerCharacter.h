#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Characters/SL_BaseCharacter.h"
#include "SL_PlayerCharacter.generated.h"

struct FInputActionValue;
class USL_DataAsset_InputConfig;
class UCameraComponent;
class USpringArmComponent;
class USL_PlayerCombatComponent;

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
	
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
private:
#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_PlayerCombatComponent> PlayerCombatComponent;
	
#pragma endregion
	
#pragma region Input
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_DataAsset_InputConfig> InputConfigDataAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed = 500.f;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_SprintStarted(const FInputActionValue& InputActionValue);
	void Input_SprintCompleted(const FInputActionValue& InputActionValue);
	
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	void Input_AbilityInputRelease(FGameplayTag InInputTag);
	
#pragma endregion
	
public:
	FORCEINLINE TObjectPtr<USL_PlayerCombatComponent> GetPlayerCombatComponent() const { return PlayerCombatComponent; }
};