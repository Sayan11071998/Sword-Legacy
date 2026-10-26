#include "AbilitySystem/Abilities/Player/SL_GA_Player_DrawOverlayWidget.h"
#include "Blueprint/UserWidget.h"
#include "Controllers/SL_PlayerController.h"
#include "Components/UI/SL_PlayerUIComponent.h"
#include "GameplayTags/SL_GameplayTags.h"
#include "Settings/SL_DeveloperSettings.h"

void USL_GA_Player_DrawOverlayWidget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const USL_DeveloperSettings* DeveloperSettings = GetDefault<USL_DeveloperSettings>();
	const bool bPlayerHUDOnStack = DeveloperSettings && !DeveloperSettings->GameWidgetMap.FindRef(SL_GameplayTags::UI_Widget_PlayerHUD).IsNull();

	if (!bPlayerHUDOnStack && OverlayWidgetClass)
	{
		if (ASL_PlayerController* PlayerController = GetPlayerControllerFromActorInfo())
		{
			UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(PlayerController, OverlayWidgetClass);
			
			if (CreatedWidget)
			{
				CreatedWidget->AddToViewport();
				
				if (USL_PlayerUIComponent* PlayerUIComponent = GetPlayerUIComponentFromActorInfo())
				{
					PlayerUIComponent->RegisterPlayerDrawnWidget(CreatedWidget);
				}
			}
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}