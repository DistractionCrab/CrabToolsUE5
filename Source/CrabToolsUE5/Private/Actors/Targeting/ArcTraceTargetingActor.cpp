#include "Actors/Targeting/ArcTraceTargetingActor.h"
#include "Actors/Targeting/ITargeter.h"
#include "Components/SplineComponent.h"

AArcTraceTargetingActor::AArcTraceTargetingActor()
{
	this->PrimaryActorTick.bCanEverTick = true;

	this->PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathDisplay"));

	this->PathSpline->SetClosedLoop(false);
	this->PathSpline->bHiddenInGame = false;
	this->PathSpline->SetVisibility(true);
	this->PathSpline->bDrawDebug = true;
	this->PathSpline->SetSimulatePhysics(false);

	this->SetRootComponent(this->PathSpline);
}

void AArcTraceTargetingActor::Tick(float DelaTime)
{
	Super::Tick(DelaTime);

	auto Base = this->GetActorLocation();
	auto Target = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());
	// Choose a halfway point between.
	auto Midpoint = (Target - Base)/2 + Base;
	// Adjust the arc to be above both points.
	Midpoint.Z = FMath::Max(Base.Z, Target.Z) + this->HeightAdjust;

	this->PathSpline->SetSplineWorldPoints({ Base, Midpoint, Target });
	this->PathSpline->SetTangentAtSplinePoint(0, (Midpoint - Base).GetSafeNormal(),
		ESplineCoordinateSpace::World, true);
	this->PathSpline->SetTangentAtSplinePoint(0, FVector::Zero(),
		ESplineCoordinateSpace::World, true);
	this->PathSpline->SetTangentAtSplinePoint(2, (Target - Midpoint).GetSafeNormal(),
		ESplineCoordinateSpace::World, true);

	float Delta = this->PathSpline->Duration/this->SampleSize;

	for (int i = 0; i < this->SampleSize; ++i)
	{
		float t0 = i * Delta;
		float t1 = (i + 1) * Delta;

		auto p1 = this->PathSpline->GetLocationAtTime(t0, ESplineCoordinateSpace::World);
		auto p2 = this->PathSpline->GetLocationAtTime(t1, ESplineCoordinateSpace::World);

		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

		TArray<AActor*> IgnoredActors;
		this->IgnoreActors(IgnoredActors);
		TraceParams.AddIgnoredActors(IgnoredActors);

		FHitResult Result(ForceInit);

		#if WITH_EDITORONLY_DATA
			if (this->bDrawDebug)
			{
				DrawDebugLine(this->GetWorld(), p1, p2, FColor(1, 1, 1), false, 0.1f, 10, 5.0f);
			}
		#endif //WITH_EDITORONLY_DATA

		bool FoundTarget = this->GetWorld()->LineTraceSingleByChannel(
			Result,
			p1,
			p1 + (p2 - p1) * (1 + this->CorrectionFactor),
			this->TraceChannel,
			TraceParams);

		if (FoundTarget)
		{
			auto CheckActor = Result.GetActor();

			this->TracedActor = CheckActor;
			this->TracedLocation = Result.ImpactPoint;

			break;
		}
		else
		{
			this->TracedActor = nullptr;
			this->TracedLocation = Base;
		}
	}
}

void AArcTraceTargetingActor::InvalidateTargetData()
{
	this->TracedActor = nullptr;
	this->TracedLocation = this->GetActorLocation();
}

void AArcTraceTargetingActor::PushTarget_Implementation()
{
	if (this->TracedActor.IsValid())
	{
		this->AddedActors.Add(this->TracedActor.Get());
		this->AddedPoints.Add(this->TracedLocation);
	}
}

void AArcTraceTargetingActor::PopTarget_Implementation()
{
	this->AddedActors.Pop();
	this->AddedPoints.Pop();
}

bool AArcTraceTargetingActor::IsValidTarget_Implementation(AActor* CheckedActor)
{
	return IsValid(CheckedActor);
}

void AArcTraceTargetingActor::IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors)
{
	IgnoredActors.Add(this->GetOwner());
}