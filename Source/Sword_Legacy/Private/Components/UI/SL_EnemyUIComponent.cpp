#include "Components/UI/SL_EnemyUIComponent.h"
#include "Widgets/SL_WidgetBase.h"

void USL_EnemyUIComponent::RegisterEnemyDrawnWidget(USL_WidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void USL_EnemyUIComponent::RemoveEnemyDrawnWidgetsIfAny()
{
	if (EnemyDrawnWidgets.IsEmpty()) return;
	
	for (USL_WidgetBase* DrawnWidget : EnemyDrawnWidgets)
	{
		if (DrawnWidget)
		{
			DrawnWidget->RemoveFromParent();
		}
	}
}