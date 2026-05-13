#include "AbilitySystem/Abilities/Player/SL_GA_Player_DrawOverlayWidget.h"

#include "Blueprint/UserWidget.h"
#include "Controllers/SL_PlayerController.h"

void USL_GA_Player_DrawOverlayWidget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (OverlayWidgetClass)
	{
		if (ASL_PlayerController* PlayerController = GetPlayerControllerFromActorInfo())
		{
			UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(PlayerController, OverlayWidgetClass);
			
			if (CreatedWidget)
			{
				CreatedWidget->AddToViewport();
			}
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
