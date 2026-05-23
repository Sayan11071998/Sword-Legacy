#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/SL_BTT_EnemyBase.h"
#include "SL_BTT_ActivateAbilityByTag.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_BTT_ActivateAbilityByTag : public USL_BTT_EnemyBase
{
	GENERATED_BODY()
	
public:
	USL_BTT_ActivateAbilityByTag();
	
	// ~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
protected:
	// ~ Begin USL_BTT_EnemyBase Interface
	virtual EBTNodeResult::Type OnEnemyExecuteTask(AAIController* OwnerController, ASL_EnemyCharacter* OwningEnemyCharacter) override;
	// ~ End USL_BTT_EnemyBase Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	FGameplayTag AbilityTagToActivate;
};