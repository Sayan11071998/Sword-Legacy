#pragma once

#include "CoreMinimal.h"
#include "Characters/SL_BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Interfaces/SL_EnemyDeathInterface.h"
#include "SL_EnemyCharacter.generated.h"

class UBoxComponent;
class UWidgetComponent;
class USL_EnemyUIComponent;
class USL_EnemyCombatComponent;
class UAnimMontage;

UCLASS()
class SWORD_LEGACY_API ASL_EnemyCharacter : public ASL_BaseCharacter, public ISL_EnemyDeathInterface
{
	GENERATED_BODY()
	
public:
	ASL_EnemyCharacter()
	
	// ~ Begin ACharacter Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// ~ End ACharacter Interface
	
	// ~ Begin ISL_PawnCombatInterface Interface
	virtual TObjectPtr<USL_PawnCombatComponent> GetPawnCombatComponent() const override;
	// ~ End ISL_PawnCombatInterface Interface
	
	// ~ Begin ISL_EnemyDeathInterface Interface
	virtual void OnEnemyDied_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InSoftNiagaraSystem) override;
	// ~ End ISL_EnemyDeathInterface Interface
	
	// ~ Begin ISL_PawnUIInterface Interface
	virtual TObjectPtr<USL_PawnUIComponent> GetPawnUIComponent() const override;
	virtual TObjectPtr<USL_EnemyUIComponent> GetEnemyUIComponent() const override;
	// ~ End ISL_PawnUIInterface Interface
	
protected:
	// ~ Begin APawn Interface
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	// ~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// ~ End UObject Interface
	
	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult		
	);
	
#pragma region Components
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USL_EnemyCombatComponent> EnemyCombatComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UBoxComponent> LeftHandCollisionBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName LeftHandCollisionBoxAttachBoneName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UBoxComponent> RightHandCollisionBox;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName RightHandCollisionBoxAttachBoneName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<USL_EnemyUIComponent> EnemyUIComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> EnemyHealthWidgetComponent;
	
#pragma endregion
	
#pragma region DissolveEffect
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TObjectPtr<UCurveFloat> DissolveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TObjectPtr<UCurveFloat> EntryRestoreCurve;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	float TotalDissolveTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	float TotalEntryRestoreTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TArray<TObjectPtr<UAnimMontage>> EntryMontagesToPlay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName LeftWeaponBoneName = FName(TEXT("weapon_l"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName DissolveParameterName = FName(TEXT("DissolveAmount"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName DissolveEdgeColorName = FName(TEXT("DissolveEdgeColor"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	FName DissolveNiagaraParticleColorName = FName(TEXT("DissolveParticleColor"));
	
#pragma endregion
	
private:
	void InitEnemyStartupData();
	
	FTimeline DissolveTimeline;
	FTimeline EntryRestoreTimeline;
    
	UFUNCTION()
	void HandleDissolveUpdate(float Value);
    
	UFUNCTION()
	void HandleDissolveFinished();

	UFUNCTION()
	void HandleEntryRestoreUpdate(float Value);
	
public:
	FORCEINLINE TObjectPtr<USL_EnemyCombatComponent> GetEnemyCombatComponent() const { return EnemyCombatComponent; }
	FORCEINLINE TObjectPtr<UBoxComponent> GetLeftHandCollisionBox() const { return LeftHandCollisionBox; }
	FORCEINLINE TObjectPtr<UBoxComponent> GetRightHandCollisionBox() const { return RightHandCollisionBox; }
};