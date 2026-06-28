#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_PickupStones.generated.h"

class ASL_StoneBase;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_PickupStones : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	USL_GA_Player_PickupStones();
	
protected:
	// ~ Begin UGameplayAbility Interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData	
	) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
	// ~ End UGameplayAbility Interface
	
	UFUNCTION(BlueprintCallable)
	void CollectStones();
	
private:
	UPROPERTY(EditDefaultsOnly)
	float BoxTraceDistance = 50.f;
	
	UPROPERTY(EditDefaultsOnly)
	FVector TraceBoxSize = FVector(100.f);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> StoneTraceChannel;
	
	UPROPERTY(EditDefaultsOnly)
	bool bDrawPersistentDebugShape = false;
	
	UPROPERTY()
	TArray<TObjectPtr<ASL_StoneBase>> CollectedStones;
};