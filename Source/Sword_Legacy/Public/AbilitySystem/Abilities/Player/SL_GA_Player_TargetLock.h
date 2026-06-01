#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "SL_GA_Player_TargetLock.generated.h"

class UInputMappingContext;
class USL_WidgetBase;

UCLASS()
class SWORD_LEGACY_API USL_GA_Player_TargetLock : public USL_PlayerGameplayAbility
{
	GENERATED_BODY()
	
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
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float BoxTraceDistance = 5000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockRotationInterpSpeed = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockMaxWalkSpeed = 150.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	FVector TraceBoxSize = FVector(5000.f, 5000.f, 300.f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceChannel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	bool bShowPersistentDebugShape = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TSubclassOf<USL_WidgetBase> TargetLockWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	TObjectPtr<UInputMappingContext> TargetLockMappingContext;
	
private:
	void TryLockOnTarget();
	void GetAvailableActorsToLock();
	TObjectPtr<AActor> GetNearestTargetFromAvailableActors(const TArray<TObjectPtr<AActor>>& InAvailableActors);
	void GetAvailableActorsAroundTarget(TArray<TObjectPtr<AActor>>& OutActorsOnLeft, TArray<TObjectPtr<AActor>>& OutActorsOnRight);
	void DrawTargetLockWidget();
	void SetTargetLockWidgetPosition();
	void InitTargetLockMovement();
	void InitTargetLockMappingContext();
	void CancelTargetLockAbility();
	void Cleanup();
	void ResetTargetLockMovement();
	void ResetTargetLockMappingContext();
	
	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> AvailableActorsToLock;
	
	UPROPERTY()
	TObjectPtr<AActor> CurrentLockedActor;
	
	UPROPERTY()
	TObjectPtr<USL_WidgetBase> DrawnTargetLockWidget;
	
	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;
	
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
};