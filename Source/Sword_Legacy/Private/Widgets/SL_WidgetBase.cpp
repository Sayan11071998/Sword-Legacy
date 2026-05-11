#include "Widgets/SL_WidgetBase.h"
#include "Interfaces/SL_PawnUIInterface.h"

void USL_WidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (ISL_PawnUIInterface* PawnUIInterface = Cast<ISL_PawnUIInterface>(OwningEnemyActor))
	{
		
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