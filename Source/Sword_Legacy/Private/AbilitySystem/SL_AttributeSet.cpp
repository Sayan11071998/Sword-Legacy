#include "AbilitySystem/SL_AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Utilities/SL_FunctionLibrary.h"
#include "Utilities/SL_GameplayTags.h"

#include "SL_DebugHelper.h"

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
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
	}
	
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());
		SetCurrentRage(NewCurrentRage);
	}
	
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();
		
		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		
		const FString DebugString = FString::Printf(
   TEXT("Old Health: %f, Damage Done: %f, New Current Health: %f"),
   OldHealth,
   DamageDone,
   NewCurrentHealth
);
		Debug::Print(DebugString, FColor::Green);
		
		if (NewCurrentHealth == 0.f)
		{
			USL_FunctionLibrary::AddGameplayToActorIfNone(Data.Target.GetAvatarActor(), SL_GameplayTags::Shared_Status_Death);
		}
	}
}