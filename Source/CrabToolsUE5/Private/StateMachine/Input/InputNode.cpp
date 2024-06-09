#include "StateMachine/Input/InputNode.h"


void UInputNode::Initialize_Implementation(UStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);

	this->PawnOwner = Cast<APawn>(POwner->GetOwner());

	if (!this->Action) { return; }

	if (this->PawnOwner.IsValid()) {
		UEnhancedInputComponent* Eic = CastChecked<UEnhancedInputComponent>(this->PawnOwner->InputComponent);
		
		if (Eic) {
			if (this->bTrigger){
				Eic->BindAction(
					this->Action, 
					ETriggerEvent::Triggered, 
					this, 
					&UInputNode::TriggerCallback_Internal);
			}

			if (this->bStart){
				Eic->BindAction(
					this->Action, 
					ETriggerEvent::Started, 
					this, 
					&UInputNode::StartCallback_Internal);	
			}

			if (this->bOngoing){
				Eic->BindAction(
					this->Action, 
					ETriggerEvent::Ongoing, 
					this, 
					&UInputNode::TriggerCallback_Internal);	
			}

			if (this->bCanceled){
				Eic->BindAction(
					this->Action, 
					ETriggerEvent::Canceled, 
					this, 
					&UInputNode::CanceledCallback_Internal);	
			}
			else {
				Eic->BindAction(
					this->Action,
					ETriggerEvent::Canceled,
					this,
					&UInputNode::FinishedCallback);
			}

			if (this->bCompleted){
				Eic->BindAction(
					this->Action, 
					ETriggerEvent::Completed, 
					this, 
					&UInputNode::CompletedCallback_Internal);	
			}
			else {
				Eic->BindAction(
					this->Action,
					ETriggerEvent::Completed,
					this,
					&UInputNode::FinishedCallback);
			}

		}
	}
}


void UInputNode::TriggerCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::TriggerCallback_Internal(const FInputActionValue& Value) {
	this->RecentValue = Value;
	if (this->Active()) {
		this->TriggerCallback(Value);
	}
}


void UInputNode::StartCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::StartCallback_Internal(const FInputActionValue& Value) {
	this->bHasStarted = true;
	this->RecentValue = Value;
	if (this->Active()) {
		this->StartCallback(Value);
	}
}

void UInputNode::OngoingCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::OngoingCallback_Internal(const FInputActionValue& Value) {
	this->bHasStarted = false;
	this->RecentValue = Value;
	if (this->Active()) {
		this->OngoingCallback(Value);
	}
}

void UInputNode::CompletedCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::CompletedCallback_Internal(const FInputActionValue& Value) {
	this->bHasStarted = false;
	this->RecentValue = Value;
	if (this->Active()) {
		this->CompletedCallback(Value);
	}
}

void UInputNode::CanceledCallback_Implementation(const FInputActionValue& Value) {}

void UInputNode::CanceledCallback_Internal(const FInputActionValue& Value) {
	this->RecentValue = Value;
	if (this->Active()) {
		this->CanceledCallback(Value);
	}
}

void UInputNode::FinishedCallback(const FInputActionValue& Value) {
	this->bHasStarted = false;
	this->RecentValue = Value;
}

FVector2D UInputNode::GetVec2D() {
	return this->RecentValue.Get<FVector2D>();
}