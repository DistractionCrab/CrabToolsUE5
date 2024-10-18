#include "Actors/Targeting/LineTraceTargetingActor.h"

ALineTraceTargetingActor::ALineTraceTargetingActor()
{
	this->PrimaryActorTick.bCanEverTick = true;
}

void ALineTraceTargetingActor::Tick(float DelaTime)
{
	Super::Tick(DelaTime);

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

	FHitResult Result(ForceInit);
	FVector Base = this->GetActorLocation();
	FVector Target = ILineTraceTargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());
	FVector FixedTarget = (1 + this->CorrectionFactor/100) * (Target - Base) + Base;

	#if WITH_EDITORONLY_DATA
		if (this->bDrawDebug)
		{
			DrawDebugLine(this->GetWorld(), Base, FixedTarget, FColor(1, 1, 1), false, 0.1f, 0, 5.0f);
		}
	#endif //WITH_EDITORONLY_DATA

	bool FoundTarget = this->GetWorld()->LineTraceSingleByChannel(
		Result,
		Base,
		FixedTarget,
		this->TraceChannel,
		TraceParams);

	if (FoundTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found a targetable thing."));
		this->TracedActor = Result.GetActor();
		this->TracedLocation = Result.ImpactPoint;
	}
	else
	{
		this->TracedActor = nullptr;
		this->TracedLocation = Base;
	}
}