#include "AnimNotifies/SL_ANS_ComboWindow.h"
#include "AbilitySystem/Abilities/Player/SL_GA_Player_ComboAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"

void USL_ANS_ComboWindow::ApplyComboWindow(const USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, bool bOpen) const
{
	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsGameWorld()) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	if (!AbilitySystemComponent) return;

	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);

	for (const FGameplayAbilitySpec& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (!AbilitySpec.IsActive()) continue;

		USL_GA_Player_ComboAttack* ComboAttack = Cast<USL_GA_Player_ComboAttack>(AbilitySpec.GetPrimaryInstance());

		if (!ComboAttack) continue;

		if (bOpen)
		{
			ComboAttack->OpenComboWindow(Montage);
		}
		else
		{
			ComboAttack->CloseComboWindow(Montage);
		}
	}
}

void USL_ANS_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ApplyComboWindow(MeshComp, Animation, true);
}

void USL_ANS_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ApplyComboWindow(MeshComp, Animation, false);
}