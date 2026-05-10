#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PawnTypes/SL_PawnEnumTypes.h"
#include "SL_ANS_ToggleWeaponCollision.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ANS_ToggleWeaponCollision : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	USL_ANS_ToggleWeaponCollision();
	
	// ~ Begin UAnimNotifyState Interface
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference	
	) override;
	
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference	
	) override;
	// ~ End UAnimNotifyState Interface
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim Notify")
	ESL_ToggleDamageType ToggleDamageType;
};