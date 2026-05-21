#include "AI/Tasks/SL_BTT_RotateToFaceTarget.h"

USL_BTT_RotateToFaceTarget::USL_BTT_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate To Face Target Actor");
	
	AnglePrecision = 10.f;
	RotationInterpSpeed = 5.f;
	
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
	
	INIT_TASK_NODE_NOTIFY_FLAGS();
	
	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
}

void USL_BTT_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (GetBlackboardAsset())
	{
		if (UBlackboardData* BBAsset = GetBlackboardAsset())
		{
			InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
		}
	}
}

uint16 USL_BTT_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}

FString USL_BTT_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();
	
	return FString::Printf(TEXT("Smoothly Rotates to Face %s Key Until the Angle Precision %s is Reached"), *KeyDescription, *FString::SanitizeFloat(AnglePrecision));
}