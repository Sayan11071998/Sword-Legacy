#include "Interfaces/SL_PawnUIInterface.h"

TObjectPtr<USL_PlayerUIComponent> ISL_PawnUIInterface::GetPlayerUIComponent() const
{
	return nullptr;
}

TObjectPtr<USL_EnemyUIComponent> ISL_PawnUIInterface::GetEnemyUIComponent() const
{
	return nullptr;
}