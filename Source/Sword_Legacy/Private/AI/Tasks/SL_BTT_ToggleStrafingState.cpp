#include "AI/Tasks/SL_BTT_ToggleStrafingState.h"
#include "AIController.h"
#include "Characters/SL_EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"

USL_BTT_ToggleStrafingState::USL_BTT_ToggleStrafingState()
{
	NodeName = TEXT("Toggle Strafing State");
	
	bShouldEnable = true;
	bShouldChangeMaxWalkSpeed = true;
	StrafingWalkSpeed = 250.f;

	InDefaultMaxWalkSpeedKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InDefaultMaxWalkSpeedKey));
}

void USL_BTT_ToggleStrafingState::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InDefaultMaxWalkSpeedKey.ResolveSelectedKey(*BBAsset);
	}
}

FString USL_BTT_ToggleStrafingState::GetStaticDescription() const
{
	if (bShouldEnable)
	{
		return FString::Printf(TEXT("Strafing State: Enable (Speed: %s)"), 
			bShouldChangeMaxWalkSpeed ? *FString::SanitizeFloat(StrafingWalkSpeed) : TEXT("No Change"));
	}
	
	return TEXT("Strafing State: Disable");
}

EBTNodeResult::Type USL_BTT_ToggleStrafingState::OnEnemyExecuteTask(AAIController* OwnerController,
	ASL_EnemyCharacter* OwningEnemyCharacter)
{
	UCharacterMovementComponent* CharacterMovement = OwningEnemyCharacter->GetCharacterMovement();
	if (!CharacterMovement) return EBTNodeResult::Failed;

	if (bShouldEnable)
	{
		CharacterMovement->bOrientRotationToMovement = false;
		if (bShouldChangeMaxWalkSpeed)
		{
			CharacterMovement->MaxWalkSpeed = StrafingWalkSpeed;
		}

		USL_FunctionLibrary::AddGameplayTagToActorIfNone(OwningEnemyCharacter, SL_GameplayTags::Enemy_Status_Strafing);
	}
	else
	{
		CharacterMovement->bOrientRotationToMovement = true;
		if (UBlackboardComponent* BlackboardComp = OwnerController->GetBlackboardComponent())
		{
			const float DefaultSpeed = BlackboardComp->GetValueAsFloat(InDefaultMaxWalkSpeedKey.SelectedKeyName);
			if (CharacterMovement->MaxWalkSpeed != DefaultSpeed)
			{
				CharacterMovement->MaxWalkSpeed = DefaultSpeed;
			}
		}

		USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(OwningEnemyCharacter, SL_GameplayTags::Enemy_Status_Strafing);
	}
	
	return EBTNodeResult::Succeeded;
}