#pragma once

#include "CoreMinimal.h"
#include "PawnTypes/SL_PawnEnumTypes.h"

class FSL_CountdownAction : public FPendingLatentAction
{
public:
	FSL_CountdownAction(
		float InTotalCountdownTime,
		float InUpdateInterval,
		float& InOutRemainingTime,
		ESL_CountDownActionOutput& InCountdownOutput,
		const FLatentActionInfo& LatentInfo	
	)
	: bNeedToCancel(false),
	  TotalCountdownTime(InTotalCountdownTime),
	  UpdateInterval(InUpdateInterval),
	  OutRemainingTime(InOutRemainingTime),
	  CountdownOutput(InCountdownOutput),
	  ExecutionFunction(LatentInfo.ExecutionFunction),
	  OutputLink(LatentInfo.Linkage),
	  CallbackTarget(LatentInfo.CallbackTarget),
	  ElapsedInterval(0.f),
	  ElapsedTimeSinceStart(0.f)
	{ }
	
private:
	bool bNeedToCancel;
	float TotalCountdownTime;
	float UpdateInterval;
	float& OutRemainingTime;
	ESL_CountDownActionOutput& CountdownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
};