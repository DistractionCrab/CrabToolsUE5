#include "StateMachine/MovementInputNode.h"


void UMovementInputNode::Initialize_Implementation(UProcStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);

	// If the owning actor has a perspective manager, store it for simple use.
	auto Persp = POwner->GetOwner()->FindComponentByClass<UPerspectiveManager>();
	this->Active = false;

	if (Persp != nullptr) {
		this->Perspective = Persp;
	}

	auto Pawn = Cast<APawn>(POwner->GetOwner());

	if (Pawn != nullptr) {
		this->PawnOwner = Pawn;

		
		if (UEnhancedInputComponent* Eic = CastChecked<UEnhancedInputComponent>(this->PawnOwner->InputComponent)) {
			Eic->BindAction(this->MoveAction, ETriggerEvent::Triggered, this, &UMovementInputNode::MoveCallback);
		}
	}
}

void UMovementInputNode::Enter_Implementation() {
	this->Active = true;
}

void UMovementInputNode::Exit_Implementation() {
	this->Active = false;
}

void UMovementInputNode::MoveCallback(const FInputActionValue& Value) {
	this->ApplyMovement(Value.Get<FVector2D>());
}

void UMovementInputNode::ApplyMovement_Implementation(FVector2D InputAxis) {
	// If our owner isn't a pawn, then we ignore it.
	if (!this->PawnOwner.IsValid()) { return; }
	if (!this->Active) { return; }

	FRotator Rotation;

	if (this->Perspective.IsValid() && Perspective->IsBound()) {
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
	this->PawnOwner->AddMovementInput(ForwardDirection, InputAxis.Y);
	this->PawnOwner->AddMovementInput(RightDirection, InputAxis.X);
}
