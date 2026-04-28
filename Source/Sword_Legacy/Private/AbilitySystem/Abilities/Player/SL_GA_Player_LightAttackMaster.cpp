#include "AbilitySystem/Abilities/Player/SL_GA_Player_LightAttackMaster.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"

USL_GA_Player_LightAttackMaster::USL_GA_Player_LightAttackMaster()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ComboResetTime = 0.3f;
}

void USL_GA_Player_LightAttackMaster::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    const float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime > ComboResetTime)
    {
        CurrentLightAttackComboCount = 1;
    }

    TObjectPtr<UAnimMontage>* MontageToPlay = LightAttackMontagesMap.Find(CurrentLightAttackComboCount);

    if (!MontageToPlay || !(*MontageToPlay))
    {
        CurrentLightAttackComboCount = 1;
        MontageToPlay = LightAttackMontagesMap.Find(CurrentLightAttackComboCount);

        if (!MontageToPlay || !(*MontageToPlay))
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }
    }

    UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, *MontageToPlay, 1.0f
    );
    PlayMontageTask->OnCompleted.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
    PlayMontageTask->OnBlendOut.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
    PlayMontageTask->OnInterrupted.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
    PlayMontageTask->OnCancelled.AddDynamic(this, &USL_GA_Player_LightAttackMaster::OnMontageCompleted);
    PlayMontageTask->ReadyForActivation();

    const int32 TotalMontages = LightAttackMontagesMap.Num();
    
    if (CurrentLightAttackComboCount == TotalMontages)
    {
        CurrentLightAttackComboCount = 1;
    }
    else
    {
        if (CurrentLightAttackComboCount == TotalMontages - 1)
        {
            AActor* AvatarActor = GetAvatarActorFromActorInfo();
            if (AvatarActor)
            {
                USL_FunctionLibrary::AddGameplayToActorIfNone(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);
            }
        }
        CurrentLightAttackComboCount++;
    }

    LastAttackTime = CurrentTime;
}

void USL_GA_Player_LightAttackMaster::OnMontageCompleted()
{
    LastAttackTime = GetWorld()->GetTimeSeconds();

    if (CurrentLightAttackComboCount == 1)
    {
        AActor* AvatarActor = GetAvatarActorFromActorInfo();
        if (AvatarActor)
        {
            USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(AvatarActor, SL_GameplayTags::Player_Status_JumpToFinisher);
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}