#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_SpawnStone_Base.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Items/Collectables/Stones/SL_StoneBase.h"

USL_GA_Enemy_SpawnStone_Base::USL_GA_Enemy_SpawnStone_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USL_GA_Enemy_SpawnStone_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo();
	if (!EnemyCharacter)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	FVector StartLocation = EnemyCharacter->GetActorLocation();
	FVector EndLocation = StartLocation + (EnemyCharacter->GetActorUpVector() * -TraceDistance);

	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(EnemyCharacter);

	bool bTraceHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		StartLocation,
		EndLocation,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	FVector RandomSpawnLocation = StartLocation;
	if (bTraceHit)
	{
		UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
			this,
			HitResult.ImpactPoint,
			RandomSpawnLocation,
			RandomSpawnRadius
		);
	}
	
	float SpawnChance = StoneSpawnChanceScalableFloat.GetValueAtLevel(GetAbilityLevel());
	if (FMath::FRand() < SpawnChance)
	{
		TSoftClassPtr<ASL_StoneBase> SelectedSoftClass = (FMath::FRand() < 0.5f) ? HealingStoneClass : RageStoneClass;

		if (!SelectedSoftClass.IsNull())
		{
			TWeakObjectPtr<USL_GA_Enemy_SpawnStone_Base> WeakThis(this);
			UAssetManager::GetStreamableManager().RequestAsyncLoad(
				SelectedSoftClass.ToSoftObjectPath(),
				FStreamableDelegate::CreateLambda(
					[WeakThis, SelectedSoftClass, RandomSpawnLocation]()
					{
						if (USL_GA_Enemy_SpawnStone_Base* StrongThis = WeakThis.Get())
						{
							StrongThis->OnStoneClassLoaded(SelectedSoftClass, RandomSpawnLocation);
						}
					}
				)
			);
			return;
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void USL_GA_Enemy_SpawnStone_Base::OnStoneClassLoaded(TSoftClassPtr<ASL_StoneBase> LoadedSoftClass,
	FVector SpawnLocation)
{
	UClass* LoadedClass = LoadedSoftClass.Get();
	UWorld* World = GetWorld();

	if (LoadedClass && World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FRotator SpawnRotation = FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f);

		World->SpawnActor<ASL_StoneBase>(
			LoadedClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Enemy_SpawnStone_Base::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bDestroyEnemyCharacterOnEnd && EnemyCharacter)
	{
		EnemyCharacter->Destroy();
	}
}