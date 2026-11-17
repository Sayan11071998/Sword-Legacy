#include "AnimNotifies/SL_ANS_SlowMotion.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void USL_ANS_SlowMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsGameWorld()) return;
	
	UGameplayStatics::SetGlobalTimeDilation(MeshComp, GlobalTimeDilationValue);
}

void USL_ANS_SlowMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World || !World->IsGameWorld()) return;
	
	UGameplayStatics::SetGlobalTimeDilation(MeshComp, 1.f);
}