#include "StateMachine/MovementStateNode.h"

void UMovementStateNode::Tick_Implementation(float DeltaTime) {
	this->ApplyMovement();
}

void UMovementStateNode::Initialize_Implementation(UStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);

	// If the owning actor has a perspective manager, store it for simple use.
	auto Persp = POwner->GetOwner()->FindComponentByClass<UPerspectiveManager>();

	if (Persp != nullptr) {
		this->Perspective = Persp;
	}

	auto Pawn = Cast<APawn>(POwner->GetOwner());

	if (Pawn != nullptr) {
		this->PawnOwner = Pawn;
	}
}

void UMovementStateNode::ApplyMovement_Implementation() {
	// If our owner isn't a pawn, then we ignore it.
	if (!this->PawnOwner.IsValid()) { return; }

	FRotator Rotation;

	if (this->Perspective.IsValid()) {
		Rotation = Perspective->GetPerspective();
	}
	else {
		Rotation = this->PawnOwner->Controller->GetControlRotation();
	}

	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	this->PawnOwner->AddMovementInput(ForwardDirection, this->InputAxis.Y);
	this->PawnOwner->AddMovementInput(RightDirection, this->InputAxis.X);
}

void UMovementStateNode::UpdateInputAxis(FVector2D FVec) {
	this->InputAxis = FVec;
}