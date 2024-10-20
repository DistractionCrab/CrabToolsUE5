#include "Actors/Targeting/LineTraceTargetingActor.h"

ALineTraceTargetingActor::ALineTraceTargetingActor()
{
	this->PrimaryActorTick.bCanEverTick = true;
}

void ALineTraceTargetingActor::Tick(float DelaTime)
{
	Super::Tick(DelaTime);

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	
	TArray<AActor*> IgnoredActors;
	this->IgnoreActors(IgnoredActors);
	TraceParams.AddIgnoredActors(IgnoredActors);

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
		auto CheckActor = Result.GetActor();
		if (this->IsValidTarget(CheckActor))
		{
			this->TracedActor = CheckActor;
			this->TracedLocation = Result.ImpactPoint;
		}
		else
		{
			this->TracedActor = nullptr;
			this->TracedLocation = Base;
		}
	}
	else
	{
		this->TracedActor = nullptr;
		this->TracedLocation = Base;
	}
}

void ALineTraceTargetingActor::InvalidateTargetData()
{
	this->TracedActor = nullptr;
	this->TracedLocation = this->GetActorLocation();
}

void ALineTraceTargetingActor::PushTarget_Implementation()
{
	if (this->TracedActor.IsValid())
	{
		this->AddedActors.Add(this->TracedActor.Get());
		this->AddedPoints.Add(this->TracedLocation);
	}
}

void ALineTraceTargetingActor::PopTarget_Implementation()
{
	this->AddedActors.Pop();
	this->AddedPoints.Pop();
}

bool ALineTraceTargetingActor::IsValidTarget_Implementation(AActor* CheckedActor)
{
	return IsValid(CheckedActor);
}

void ALineTraceTargetingActor::IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors)
{
	IgnoredActors.Add(this->GetOwner());
}