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

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

	TArray<AActor*> IgnoredActors;
	this->IgnoreActors(IgnoredActors);
	TraceParams.AddIgnoredActors(IgnoredActors);

	FHitResult Result(ForceInit);

	this->Base = this->GetActorLocation();
	this->Target = ITargeterInterface::Execute_GetEndPoint(this->GetUsingActorNative());
	// Choose a halfway point between.
	this->Midpoint = (Target - Base)/2 + Base;
	// Adjust the arc to be above both points.
	this->Midpoint.Z = FMath::Max(Base.Z, Target.Z) + this->HeightAdjust;

	this->PathSpline->SetSplineWorldPoints({ Base, this->Midpoint, Target });
	this->PathSpline->SetTangentAtSplinePoint(0, (this->Midpoint - Base).GetSafeNormal(),
		ESplineCoordinateSpace::World, true);
	this->PathSpline->SetTangentAtSplinePoint(0, FVector::Zero(),
		ESplineCoordinateSpace::World, true);
	this->PathSpline->SetTangentAtSplinePoint(2, (this->Target - Midpoint).GetSafeNormal(),
		ESplineCoordinateSpace::World, true);
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