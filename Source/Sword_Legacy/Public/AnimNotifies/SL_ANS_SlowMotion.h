#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SL_ANS_SlowMotion.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_ANS_SlowMotion : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float GlobalTimeDilationValue = 0.2f;
};