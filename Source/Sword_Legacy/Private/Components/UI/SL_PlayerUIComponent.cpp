#include "Components/UI/SL_PlayerUIComponent.h"
#include "Blueprint/UserWidget.h"

void USL_PlayerUIComponent::RegisterPlayerDrawnWidget(UUserWidget* InWidgetToRegister)
{
	if (InWidgetToRegister)
	{
		PlayerDrawnWidgets.AddUnique(InWidgetToRegister);
	}
}

void USL_PlayerUIComponent::RemovePlayerDrawnWidgetsIfAny()
{
	if (PlayerDrawnWidgets.IsEmpty()) return;
	
	for (UUserWidget* DrawnWidget : PlayerDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}
	
	PlayerDrawnWidgets.Empty();
}
