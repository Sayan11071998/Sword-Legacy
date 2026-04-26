#include "AbilitySystem/Abilities/Shared/SL_GA_Shared_SpawnWeapon.h"
#include "Items/Weapons/SL_WeaponBase.h"
#include "Components/Combat/SL_PawnCombatComponent.h"

void USL_GA_Shared_SpawnWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarPawn;
	SpawnParams.Instigator = AvatarPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	ASL_WeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<ASL_WeaponBase>(WeaponClassToSpawn, FTransform::Identity, SpawnParams);

	if (IsValid(SpawnedWeapon))
	{
		USkeletalMeshComponent* SkeletalMesh = ActorInfo->SkeletalMeshComponent.Get();
		
		if (SkeletalMesh)
		{
			const FAttachmentTransformRules AttachRules(
				EAttachmentRule::SnapToTarget,
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepWorld,
				true
			);
			
			SpawnedWeapon->AttachToComponent(SkeletalMesh, AttachRules, SocketNameToAttachTo);
		}
		
		if (USL_PawnCombatComponent* CombatComponent = GetPawnCombatComponentFromActorInfo())
		{
			CombatComponent->RegisterSpawnedWeapon(WeaponTagToRegister, SpawnedWeapon, bRegisterAsEquippedWeapon);
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}