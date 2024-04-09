// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PatrolPath.h"

// Sets default values
APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(this->Root);
}

// Called when the game starts or when spawned
void APatrolPath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APatrolPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

AActor* APatrolPath::FindClosestPoint(AActor* Patroller) {
	if (Patroller == nullptr) {
		return this;
	}

	AActor* Goal = Patroller;
	float Dist = this->LostDistance * this->LostDistance;

	for (AActor* p : this->PatrolPoints) {
		float DSquared = FVector::DistSquared(p->GetActorLocation(), Patroller->GetActorLocation());
		if (DSquared < Dist) {
			Goal = p;
			Dist = DSquared;
		}
	}
	return Goal;
}

int APatrolPath::FindClosestIndex(AActor* Patroller) {
	if (Patroller == nullptr) {
		return -1;
	} else if (this->PatrolPoints.Num() == 0) {
		return -1;
	}

	int Goal = 0;
	float Dist = this->LostDistance * this->LostDistance;

	for (int i = 0; i < this->PatrolPoints.Num(); ++i) {
		AActor* p = this->PatrolPoints[i];
		float DSquared = FVector::DistSquared(p->GetActorLocation(), Patroller->GetActorLocation());
		if (DSquared < Dist) {
			Goal = i;
			Dist = DSquared;
		}
	}

	return Goal;
}

AActor* FPatrolPathState::GetNextTarget(APatrolPath* Path) {
	auto PointCount = Path->PatrolPoints.Num();

	if (Path && PointCount > 0) {
		auto Next = Path->PatrolPoints[this->Index];

		this->Index %= PointCount;
		return Next;
	}
	else {
		return nullptr;
	}
}

AActor* UPatrolPathLibrary::GetNextTarget(FPatrolPathState& State, APatrolPath* Path) {
	return State.GetNextTarget(Path);
}