// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ElevatorSystem.h"

// Sets default values
AElevatorSystem::AElevatorSystem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ElevatorMotionTimeline"));

	
}

// Called when the game starts or when spawned
void AElevatorSystem::BeginPlay()
{
	Super::BeginPlay();

	this->MovementTimeline->SetLooping(false);
	this->MovementTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	this->MovementTrack.BindDynamic(this, &AElevatorSystem::UpdatePosition);
	this->EndMovementTrack.BindDynamic(this, &AElevatorSystem::FinishedMovement);

	if (this->MovementCurve != nullptr) {
		this->MovementTimeline->AddInterpFloat(this->MovementCurve, MovementTrack);
		this->MovementTimeline->SetTimelineFinishedFunc(EndMovementTrack);
	}
}



void AElevatorSystem::Step() {
	if (this->HasTwoTargets()) {
		// If we're at the start, then we always need to go "upwards."
		if (this->PositionIndex == 0) {
			this->MovementSequence.Add(0);
			this->MovementSequence.Add(1);
		}
		else if (this->PositionIndex == this->Targets.Num() - 1) {
			this->MovementSequence.Add(this->Targets.Num() - 1);
			this->MovementSequence.Add(this->Targets.Num() - 2);
		}
		else {
			if (this->Reverse) {
				this->MovementSequence.Add(this->PositionIndex);
				this->MovementSequence.Add(this->PositionIndex - 1);
			}
			else {
				this->MovementSequence.Add(this->PositionIndex);
				this->MovementSequence.Add(this->PositionIndex + 1);
			}
		}
	}

	this->StartMovement();
}

void AElevatorSystem::UpdatePosition(float alpha) {
	
	alpha = FMath::Clamp(alpha, 0, 1);

	if (this->HasTwoPoints() && this->HasTwoTargets()) {
		this->SetActorLocation(this->GetMovementStart() + this->GetMovementDirection() * alpha);
	}
}

void AElevatorSystem::FinishedMovement() {
	int Num = this->MovementSequence.Num();
	int i0 = MovementSequence[0];
	int i1 = MovementSequence[1];
	if (Num <= 2) {
		// If we're at the final index, we can only reverse.
		if (i1 == this->Targets.Num() - 1) {
			this->Reverse = true;
		}
		// If we're went to the start, then no reversal.
		else if (i1 == 0) {
			this->Reverse = false;
		}
		// Otherwise, take the order of the indices.
		else {
			this->Reverse = i1 < i0;
		}
		this->PositionIndex = i1;
		this->MovementSequence.Empty();
		this->PostFinishCallback();
	}
	else {
		this->MovementSequence.HeapPop(this->PositionIndex, true);
		this->StartMovement();
	}
}

FVector AElevatorSystem::GetMovementStart() {
	if (this->HasTwoPoints() && this->HasTwoTargets()) {
		return this->Targets[this->MovementSequence[0]]->GetActorLocation();
	}
	else {
		return this->GetActorLocation();
	}
	
}

FVector AElevatorSystem::GetMovementDirection() {

	if (this->HasTwoPoints() && this->HasTwoTargets()) {
		FVector p1 = this->Targets[this->MovementSequence[1]]->GetActorLocation();
		FVector p0 = this->Targets[this->MovementSequence[0]]->GetActorLocation();
		return p1 - p0;
	}
	else {
		return FVector::ZeroVector;
	}
}

void AElevatorSystem::PostFinishCallback_Implementation() {}


void AElevatorSystem::GoToIndex(int i) {
	if (i >= 0 && i < this->Targets.Num()) {
		this->MovementSequence.Add(this->PositionIndex);
		this->MovementSequence.Add(i);
		this->StartMovement();
	}	
}