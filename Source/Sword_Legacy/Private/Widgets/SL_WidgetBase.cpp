#include "Widgets/SL_WidgetBase.h"
#include "Components/UI/SL_EnemyUIComponent.h"
#include "Interfaces/SL_PawnUIInterface.h"

void USL_WidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (ISL_PawnUIInterface* PawnUIInterface = Cast<ISL_PawnUIInterface>(OwningEnemyActor))
	{
		USL_EnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();
		checkf(EnemyUIComponent, TEXT("Failed to extract an Enemy UI Component from %s"), OwningEnemyActor->GetActorNameOrLabel());
		
		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}

void USL_WidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ISL_PawnUIInterface* PawnUIInterface = Cast<ISL_PawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (USL_PlayerUIComponent* PlayerUIComponent = PawnUIInterface->GetPlayerUIComponent())
		{
			BP_OnOwningPlayerUIComponentInitialized(PlayerUIComponent);
		}
	}
}