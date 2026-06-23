#include "Utilities/SL_CountdownAction.h"

void FSL_CountdownAction::CancelAction()
{
	bNeedToCancel = true;
}