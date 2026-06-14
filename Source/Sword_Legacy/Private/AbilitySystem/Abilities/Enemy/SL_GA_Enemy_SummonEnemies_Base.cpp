#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_SummonEnemies_Base.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/AbilityTasks/SL_AbilityTask_WaitSpawnEnemies.h"
#include "Characters/SL_EnemyCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Enemy_SummonEnemies_Base::USL_GA_Enemy_SummonEnemies_Base()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USL_GA_Enemy_SummonEnemies_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	if (MontageToPlay)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay
		);
		PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Enemy_SummonEnemies_Base::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Enemy_SummonEnemies_Base::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Enemy_SummonEnemies_Base::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Enemy_SummonEnemies_Base::OnMontageFinished);
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	USL_AbilityTask_WaitSpawnEnemies* WaitSpawnTask = USL_AbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(
		this,
		SL_GameplayTags::Enemy_Event_SummonEnemies,
		SoftEnemyClassToSpawn,
		NumberOfEnemiesToSpawn,
		EnemyCharacter->GetActorLocation(),
		RandomSpawnRadius
	);
	
	WaitSpawnTask->OnSpawnFinished.AddDynamic(this, &USL_GA_Enemy_SummonEnemies_Base::OnSpawnFinished);
	WaitSpawnTask->ReadyForActivation();
}

void USL_GA_Enemy_SummonEnemies_Base::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USL_GA_Enemy_SummonEnemies_Base::OnSpawnFinished(const TArray<ASL_EnemyCharacter*>& SpawnedEnemies)
{
	if (ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
	{
		if (AAIController* AIController = Cast<AAIController>(EnemyCharacter->GetController()))
		{
			if (UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent())
			{
				Blackboard->SetValueAsBool(FName(TEXT("HasSpawnedEnemies")), true);
			}
		}
	}
}