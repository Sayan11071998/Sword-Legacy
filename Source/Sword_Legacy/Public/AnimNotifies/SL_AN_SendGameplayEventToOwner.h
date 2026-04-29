#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SL_AN_SendGameplayEventToOwner.generated.h"

UCLASS()
class SWORD_LEGACY_API USL_AN_SendGameplayEventToOwner : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAnimNotify Interface
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// ~ End UAnimNotify Interface
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	FGameplayTag EventTag;
};