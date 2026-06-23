#include "Utilities/SL_CountdownAction.h"

void FSL_CountdownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{
		CountdownOutput = ESL_CountDownActionOutput::Cancelled;
		
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		
		return;
	}
	
	if (ElapsedTimeSinceStart >= TotalCountdownTime)
	{
		CountdownOutput = ESL_CountDownActionOutput::Completed;
		
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		
		return;
	}
	
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();
		
		OutRemainingTime = TotalCountdownTime - ElapsedTimeSinceStart;
		
		CountdownOutput = ESL_CountDownActionOutput::Updated;
		
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
		
		ElapsedInterval = 0.f;
	}
}

void FSL_CountdownAction::CancelAction()
{
	bNeedToCancel = true;
}