#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SL_ANS_ComboWindow.generated.h"

class USL_GA_Player_ComboAttack;

UCLASS(meta = (DisplayName = "Combo Window"))
class SWORD_LEGACY_API USL_ANS_ComboWindow : public UAnimNotifyState
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

private:
	void ApplyComboWindow(const USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, bool bOpen) const;
};