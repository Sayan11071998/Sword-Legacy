#include "AnimNotifies/SL_AN_IncomingAttackWarning.h"
#include "AbilitySystem/Abilities/SL_PlayerGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"

void USL_AN_IncomingAttackWarning::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsGameWorld()) return;

	AActor* Attacker = MeshComp->GetOwner();
	if (!Attacker || DetectionRadius <= 0.f) return;

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Attacker);
	USL_PlayerGameplayAbility* AttackAbility = nullptr;

	if (AbilitySystemComponent)
	{
		AttackAbility = Cast<USL_PlayerGameplayAbility>(AbilitySystemComponent->GetAnimatingAbility());

		if (!AttackAbility)
		{
			for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
			{
				if (!AbilitySpec.IsActive()) continue;

				for (UGameplayAbility* Instance : AbilitySpec.GetAbilityInstances())
				{
					USL_PlayerGameplayAbility* PlayerAbility = Cast<USL_PlayerGameplayAbility>(Instance);

					if (PlayerAbility && PlayerAbility->IsActive() && PlayerAbility->GetCurrentMontage() == Animation)
					{
						AttackAbility = PlayerAbility;
						break;
					}
				}

				if (AttackAbility) break;
			}
		}
	}

	TArray<FOverlapResult> Overlaps;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(IncomingAttackWarning), false, Attacker);
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	World->OverlapMultiByObjectType(
		Overlaps,
		Attacker->GetActorLocation(),
		FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(DetectionRadius),
		QueryParams
	);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		ASL_EnemyCharacter* Enemy = Cast<ASL_EnemyCharacter>(Overlap.GetActor());

		if (!Enemy || !Enemy->RespondToIncomingAttack(Attacker)) continue;

		if (AttackAbility)
		{
			AttackAbility->WarnEnemyOfIncomingAttack(Enemy);
		}
	}
}

FString USL_AN_IncomingAttackWarning::GetNotifyName_Implementation() const
{
	return TEXT("Incoming Attack Warning");
}