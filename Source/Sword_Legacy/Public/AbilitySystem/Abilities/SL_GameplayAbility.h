#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SL_GameplayAbility.generated.h"

UENUM(BlueprintType)
enum class ESL_AbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UCLASS()
class SWORD_LEGACY_API USL_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, Category = "CharacterAbility")
	ESL_AbilityActivationPolicy AbilityActivationPolicy = ESL_AbilityActivationPolicy::OnTriggered;
};