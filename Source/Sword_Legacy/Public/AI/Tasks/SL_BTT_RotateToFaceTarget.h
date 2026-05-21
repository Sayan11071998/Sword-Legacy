#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/SL_BTT_EnemyBase.h"
#include "SL_BTT_RotateToFaceTarget.generated.h"

struct FRotateToFaceTargetTaskMemory
{
	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<AActor> TargetActor;
	
	bool IsValid() const
	{
		return OwningPawn.IsValid() && TargetActor.IsValid();
	}
	
	void Reset()
	{
		OwningPawn.Reset();
		TargetActor.Reset();
	}
};

UCLASS()
class SWORD_LEGACY_API USL_BTT_RotateToFaceTarget : public USL_BTT_EnemyBase
{
	GENERATED_BODY()
	
public:
	USL_BTT_RotateToFaceTarget();
	
	// ~ Begin UBTNode Interface
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual FString GetStaticDescription() const override;
	// ~ End UBTNode Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "FaceTarget")
	float AnglePrecision;
	
	UPROPERTY(EditAnywhere, Category = "FaceTarget")
	float RotationInterpSpeed;
	
	UPROPERTY(EditAnywhere, Category = "FaceTarget")
	FBlackboardKeySelector InTargetToFaceKey;
};