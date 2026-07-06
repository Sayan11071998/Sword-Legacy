#include "AbilitySystem/SL_AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"
#include "Interfaces/SL_PawnUIInterface.h"
#include "Components/UI/SL_PawnUIComponent.h"
#include "Components/UI/SL_PlayerUIComponent.h"

USL_AttributeSet::USL_AttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
}

void USL_AttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	AActor* TargetAvatar = Data.Target.GetAvatarActor();
	if (!TargetAvatar)
	{
		return;
	}
	
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<ISL_PawnUIInterface>(TargetAvatar);
	}
	
	USL_PawnUIComponent* PawnUIComponent = CachedPawnUIInterface.IsValid() ? CachedPawnUIInterface->GetPawnUIComponent() : nullptr;
	
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		if (PawnUIComponent)
		{
			PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
		}
	}
	
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());
		SetCurrentRage(NewCurrentRage);
		
		if (GetCurrentRage() == GetMaxRage())
		{
			USL_FunctionLibrary::AddGameplayTagToActorIfNone(TargetAvatar, SL_GameplayTags::Player_Status_Rage_Full);
		}
		else if (GetCurrentRage() == 0.f)
		{
			USL_FunctionLibrary::AddGameplayTagToActorIfNone(TargetAvatar, SL_GameplayTags::Player_Status_Rage_None);
		}
		else
		{
			USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(TargetAvatar, SL_GameplayTags::Player_Status_Rage_Full);
			USL_FunctionLibrary::RemoveGameplayTagFromActorIfFound(TargetAvatar, SL_GameplayTags::Player_Status_Rage_None);
		}
		
		if (CachedPawnUIInterface.IsValid())
		{
			if (USL_PlayerUIComponent* PlayerUIComponent = CachedPawnUIInterface->GetPlayerUIComponent())
			{
				PlayerUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
			}
		}
	}
	
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();
		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
		
		SetCurrentHealth(NewCurrentHealth);
		
		if (PawnUIComponent)
		{
			PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
		}
		
		if (GetCurrentHealth() == 0.f)
		{
			USL_FunctionLibrary::AddGameplayTagToActorIfNone(TargetAvatar, SL_GameplayTags::Shared_Status_Death);
		}
	}
}