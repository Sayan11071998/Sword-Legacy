#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SL_GameplayAbility.h"
#include "SL_GA_Shared_SpawnWeapon.generated.h"

class ASL_WeaponBase;

UCLASS()
class SWORD_LEGACY_API USL_GA_Shared_SpawnWeapon : public USL_GameplayAbility
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
	// ~ End UGameplayAbility Interface
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ASL_WeaponBase> WeaponClassToSpawn;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	FName SocketNameToAttachTo;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	FGameplayTag WeaponTagToRegister;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	bool bRegisterAsEquippedWeapon = false;
};