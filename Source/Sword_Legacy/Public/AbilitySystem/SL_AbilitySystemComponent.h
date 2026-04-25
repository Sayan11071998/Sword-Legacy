#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SL_AbilitySystemComponent.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);
};