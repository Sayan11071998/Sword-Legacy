#include "AnimNotifies/SL_ANS_SlowMotion.h"
#include "Kismet/GameplayStatics.h"

void USL_ANS_SlowMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	UGameplayStatics::SetGlobalTimeDilation(MeshComp, GlobalTimeDilationValue);
}

void USL_ANS_SlowMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	UGameplayStatics::SetGlobalTimeDilation(MeshComp, 1.f);
}