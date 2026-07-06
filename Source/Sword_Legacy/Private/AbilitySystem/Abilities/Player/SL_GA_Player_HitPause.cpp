#include "AbilitySystem/Abilities/Player/SL_GA_Player_HitPause.h"
#include "Controllers/SL_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Player_HitPause::USL_GA_Player_HitPause()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SL_GameplayTags::Player_Event_HitPause;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void USL_GA_Player_HitPause::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), HitPauseTimeDilation);
	
	FTimerHandle HitPauseTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(HitPauseTimerHandle, [this]()
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
		
		if (HitPauseCameraShakeClass)
		{
			if (ASL_PlayerController* PlayerController = Cast<ASL_PlayerController>(GetCurrentActorInfo()->PlayerController.Get()))
			{
				PlayerController->ClientStartCameraShake(HitPauseCameraShakeClass);
			}
		}
		
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}, HitPauseTimeDuration, false);
}