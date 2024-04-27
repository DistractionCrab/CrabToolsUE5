#include "StateMachine/Input/SprintMovementNode.h"

void USprintMovementNode::Initialize_Implementation(UStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);

	auto Pawn = this->GetPawn();

	if (Pawn) {
		UEnhancedInputComponent* Eic = CastChecked<UEnhancedInputComponent>(Pawn->InputComponent);		
		if (Eic) {
			if (this->SprintAction) {
				Eic->BindAction<USprintMovementNode, bool>(
					this->SprintAction, 
					ETriggerEvent::Started, 
					this, 
					&USprintMovementNode::ApplySprint,
					true);

				Eic->BindAction<USprintMovementNode, bool>(
					this->SprintAction, 
					ETriggerEvent::Completed, 
					this, 
					&USprintMovementNode::ApplySprint,
					false);

				Eic->BindAction<USprintMovementNode, bool>(
					this->SprintAction, 
					ETriggerEvent::Canceled, 
					this, 
					&USprintMovementNode::ApplySprint,
					false);
			}

			if (this->WalkAction) {
				Eic->BindAction<USprintMovementNode, bool>(
					this->WalkAction,
					ETriggerEvent::Started,
					this,
					&USprintMovementNode::ApplyWalk,
					true);

				Eic->BindAction<USprintMovementNode, bool>(
					this->WalkAction,
					ETriggerEvent::Completed,
					this,
					&USprintMovementNode::ApplyWalk,
					false);

				Eic->BindAction<USprintMovementNode, bool>(
					this->WalkAction,
					ETriggerEvent::Canceled,
					this,
					&USprintMovementNode::ApplyWalk,
					false);
			}
		}	
	}

	
}


void USprintMovementNode::ApplyMovement_Implementation(FVector2D InputAxis) {
	if (this->bForceSprint) {
		Super::ApplyMovement_Implementation(InputAxis * this->SprintRatio);
	} else if (this->bForceWalk) {
		Super::ApplyMovement_Implementation(InputAxis * this->WalkRatio);
	} else {
		Super::ApplyMovement_Implementation(InputAxis * this->JogRatio);
	}
}
