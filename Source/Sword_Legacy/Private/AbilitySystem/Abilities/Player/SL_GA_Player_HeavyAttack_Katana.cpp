#include "AbilitySystem/Abilities/Player/SL_GA_Player_HeavyAttack_Katana.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/SL_PlayerCombatComponent.h"
#include "Items/Weapons/SL_PlayerWeapon.h"
#include "Characters/SL_PlayerCharacter.h"
#include "Items/Projectiles/SL_ProjectileBase.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Player_HeavyAttack_Katana::USL_GA_Player_HeavyAttack_Katana()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	ProjectileSpawnSocketName = FName(TEXT("RageSlashSocket"));
	RageDamageMultiplier = 2.0f;
	RageComboCount = 1;
}

void USL_GA_Player_HeavyAttack_Katana::WhileRageActive_Implementation()
{
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		SL_GameplayTags::Shared_Event_SpawnProjectile,
		nullptr,
		true,
		true
	);
	WaitEventTask->EventReceived.AddDynamic(this, &USL_GA_Player_HeavyAttack_Katana::OnSpawnProjectileEventReceived);
	WaitEventTask->ReadyForActivation();
}

void USL_GA_Player_HeavyAttack_Katana::OnSpawnProjectileEventReceived(FGameplayEventData Payload)
{
	ASL_PlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	if (!PlayerCharacter || !ProjectileClass) return;

	USL_PlayerCombatComponent* CombatComponent = GetPlayerCombatComponentFromActorInfo();
	if (!CombatComponent) return;

	ASL_PlayerWeapon* EquippedWeapon = CombatComponent->GetPlayerCurrentEquippedWeapon();
	if (!EquippedWeapon) return;

	UStaticMeshComponent* WeaponMesh = EquippedWeapon->GetWeaponMesh();
	if (!WeaponMesh) return;

	FTransform SocketTransform = WeaponMesh->GetSocketTransform(ProjectileSpawnSocketName);
	FVector SpawnLocation = SocketTransform.GetLocation();

	FVector ForwardVector = PlayerCharacter->GetActorForwardVector();
	FRotator SpawnRotation = ForwardVector.Rotation();

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = PlayerCharacter;
	SpawnParams.Instigator = PlayerCharacter;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ASL_ProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<ASL_ProjectileBase>(
		ProjectileClass,
		SpawnTransform,
		SpawnParams
	);

	if (SpawnedProjectile && ProjectileDamageEffectClass)
	{
		const float RawLevelDamage = CombatComponent->GetPlayerCurrentEquippedWeaponDamageAtLevel(static_cast<float>(GetAbilityLevel()) * RageDamageMultiplier);

		const FGameplayEffectSpecHandle DamageSpecHandle = MakePlayerDamageEffectSpecHandle(
			ProjectileDamageEffectClass,
			RawLevelDamage,
			SL_GameplayTags::Player_SetByCaller_AttackType_Heavy,
			RageComboCount
		);

		SpawnedProjectile->ProjectileDamageEffectSpecHandle = DamageSpecHandle;
	}
}