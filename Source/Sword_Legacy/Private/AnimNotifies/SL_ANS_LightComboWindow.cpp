#include "AnimNotifies/SL_ANS_LightComboWindow.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightAttackMaster.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"

namespace
{
	USL_GA_Player_LightAttackMaster* FindActiveLightAttack(const USkeletalMeshComponent* MeshComp)
	{
		const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
		if (!World || !World->IsGameWorld()) return nullptr;

		AActor* Owner = MeshComp->GetOwner();
		if (!Owner) return nullptr;

		const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
		if (!AbilitySystemComponent) return nullptr;

		for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (!AbilitySpec.IsActive()) continue;

			if (USL_GA_Player_LightAttackMaster* LightAttack = Cast<USL_GA_Player_LightAttackMaster>(AbilitySpec.GetPrimaryInstance()))
			{
				return LightAttack;
			}
		}

		return nullptr;
	}
}

void USL_ANS_LightComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (USL_GA_Player_LightAttackMaster* LightAttack = FindActiveLightAttack(MeshComp))
	{
		LightAttack->OpenComboWindow(Cast<UAnimMontage>(Animation));
	}
}

void USL_ANS_LightComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (USL_GA_Player_LightAttackMaster* LightAttack = FindActiveLightAttack(MeshComp))
	{
		LightAttack->CloseComboWindow(Cast<UAnimMontage>(Animation));
	}
}