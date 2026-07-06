#include "AbilitySystem/Abilities/Enemy/SL_GA_Enemy_DrawBossBar_Base.h"
#include "Widgets/SL_WidgetBase.h"
#include "Characters/SL_EnemyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/UI/SL_EnemyUIComponent.h"

USL_GA_Enemy_DrawBossBar_Base::USL_GA_Enemy_DrawBossBar_Base()
{
	AbilityActivationPolicy = ESL_AbilityActivationPolicy::OnGiven;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USL_GA_Enemy_DrawBossBar_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!BossHealthBarWidgetClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	APlayerController* PC = ActorInfo->PlayerController.Get();
	if (!PC)
	{
		if (APawn* OwningPawn = Cast<APawn>(ActorInfo->AvatarActor.Get()))
		{
			PC = Cast<APlayerController>(OwningPawn->GetController());
		}
	}
	
	if (!PC)
	{
		PC = GetWorld()->GetFirstPlayerController();
	}
	
	USL_WidgetBase* CreatedWidget = CreateWidget<USL_WidgetBase>(PC, BossHealthBarWidgetClass);
	if (CreatedWidget)
	{
		if (UTextBlock* BossNameText = Cast<UTextBlock>(CreatedWidget->GetWidgetFromName(BossNameTextBlockName)))
		{
			BossNameText->SetText(BossName);
		}

		CreatedWidget->AddToViewport();
		if (ASL_EnemyCharacter* EnemyCharacter = GetEnemyCharacterFromActorInfo())
		{
			CreatedWidget->InitEnemyCreatedWidget(EnemyCharacter);
			if (USL_EnemyUIComponent* EnemyUIComponent = EnemyCharacter->GetEnemyUIComponent())
			{
				EnemyUIComponent->RegisterEnemyDrawnWidget(CreatedWidget);
			}
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}