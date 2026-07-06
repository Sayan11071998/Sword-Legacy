#include "AI/Decorators/SL_BTD_DoesActorHaveTag.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

USL_BTD_DoesActorHaveTag::USL_BTD_DoesActorHaveTag()
{
	NodeName = TEXT("Does Actor Have Tag");
	
	bInverseConditionCheck = false;

	InActorKeyToCheck.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InActorKeyToCheck), AActor::StaticClass());
}

void USL_BTD_DoesActorHaveTag::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InActorKeyToCheck.ResolveSelectedKey(*BBAsset);
	}
}

FString USL_BTD_DoesActorHaveTag::GetStaticDescription() const
{
	const FString KeyDescription = InActorKeyToCheck.SelectedKeyName.ToString();
	const FString TagDescription = TagToCheck.IsValid() ? TagToCheck.ToString() : TEXT("None");
	const FString InverseText = bInverseConditionCheck ? TEXT("NOT ") : TEXT("");
	
	return FString::Printf(TEXT("Condition: Expect %sActor %s to have tag %s"), *InverseText, *KeyDescription, *TagDescription);
}

bool USL_BTD_DoesActorHaveTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return bInverseConditionCheck;
	}
	
	bool bHasTag = false;

	if (AActor* ActorToCheck = Cast<AActor>(BlackboardComp->GetValueAsObject(InActorKeyToCheck.SelectedKeyName)))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToCheck))
		{
			bHasTag = ASC->HasMatchingGameplayTag(TagToCheck);
		}
	}

	return bInverseConditionCheck ? !bHasTag : bHasTag;
}