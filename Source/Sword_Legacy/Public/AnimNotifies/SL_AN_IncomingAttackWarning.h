#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SL_AN_IncomingAttackWarning.generated.h"

UCLASS(meta = (DisplayName = "Incoming Attack Warning"))
class SWORD_LEGACY_API USL_AN_IncomingAttackWarning : public UAnimNotify
{
	GENERATED_BODY()

public:
	// ~ Begin UAnimNotify Interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
	// ~ End UAnimNotify Interface

protected:
	UPROPERTY(EditAnywhere, Category = "Incoming Attack", meta = (ClampMin = "0.0"))
	float DetectionRadius = 1500.f;
};