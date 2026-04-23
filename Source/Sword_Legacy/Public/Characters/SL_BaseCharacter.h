#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SL_BaseCharacter.generated.h"

class USL_AttributeSet;
class USL_AbilitySystemComponent;

UCLASS()
class SWORD_LEGACY_API ASL_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASL_BaseCharacter();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<USL_AbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<USL_AttributeSet> AttributeSet;
	
public:
	FORCEINLINE TObjectPtr<USL_AbilitySystemComponent> GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE TObjectPtr<USL_AttributeSet> GetAttributeSet() const { return AttributeSet; }
};