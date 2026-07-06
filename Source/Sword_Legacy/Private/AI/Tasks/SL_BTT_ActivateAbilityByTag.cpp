#include "AI/Tasks/SL_BTT_ActivateAbilityByTag.h"
#include "Characters/SL_EnemyCharacter.h"
#include "AbilitySystem/SL_AbilitySystemComponent.h"

USL_BTT_ActivateAbilityByTag::USL_BTT_ActivateAbilityByTag()
{
	NodeName = TEXT("Activate Ability By Tag");
}

FString USL_BTT_ActivateAbilityByTag::GetStaticDescription() const
{
	const FString TagDesc = AbilityTagToActivate.IsValid() ? AbilityTagToActivate.ToString() : TEXT("None");
	
	return FString::Printf(TEXT("Activate Ability: %s"), *TagDesc);
}

EBTNodeResult::Type USL_BTT_ActivateAbilityByTag::OnEnemyExecuteTask(AAIController* OwnerController,
	ASL_EnemyCharacter* OwningEnemyCharacter)
{
	USL_AbilitySystemComponent* ASC = OwningEnemyCharacter->GetCharacterAbilitySystemComponent();
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	if (AbilityTagToActivate.IsValid())
	{
		const bool bSuccess = ASC->TryActivateAbilityByTag(AbilityTagToActivate);
		return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}