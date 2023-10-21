#include "StateMachine/ProcStateMachine.h"

#pragma region StateMachine Code

void UProcStateMachine::Initialize_Implementation(AActor* POwner) {
	this->Owner = POwner;
	for (auto& pair : this->Graph) {
		auto& StateName = pair.Key;
		auto& StateData = pair.Value;
		/*
		if (StateData.Node == nullptr) {
			StateData.Node = NewObject<UStateNode>(
				this, 
				StateData.NodeClass, 
				StateName, 
				RF_NoFlags, 
				StateData.NodeClass.GetDefaultObject(), 
				true);			
		}
		*/
		StateData.Node->Initialize(this);
	}
}

AActor* UProcStateMachine::GetOwner() {
	return this->Owner;
}

void UProcStateMachine::UpdateState(FName Name) {
	if (this->Graph.Contains(Name)) {
		auto CurrentState = this->GetCurrentState();
		auto TID = this->TRANSITION.EnterTransition();

		// Alert all listeners, and if one of them changes the state, return.
		for (const auto& Listener: this->StateChangeEvents) {
			Listener.ExecuteIfBound(this->CurrentStateName, Name);
			if (!this->TRANSITION.Valid(TID)) {
				return;
			}
		}
		// Only transition if no other state update has occurred.
		if (this->TRANSITION.Valid(TID)) {
			CurrentState.Node->Exit();
			this->CurrentStateName = Name;
			CurrentState.Node->Enter();
		}
	}
}

void UProcStateMachine::Tick(float DeltaTime) {
	if (this->CurrentStateName != NAME_None) {
		this->GetCurrentState().Node->Tick(DeltaTime);
	}
}

void UProcStateMachine::ListenForChange(const FStateChangeDispatcher& obs) {
	this->StateChangeEvents.Add(obs);
}

void UProcStateMachine::Reset() {
	this->CurrentStateName = this->StartState;
}

void UProcStateMachine::Event(FName EName) {
	// Need to validate possible transitions.
	auto TID = this->TRANSITION.EnterTransition();
	auto CurrentState = this->GetCurrentState();
	CurrentState.Node->Event(EName);

	// If the current node's event code didn't change the graph's state, then we check for
	// static event transitions.
	if (this->TRANSITION.Valid(TID)) {
		if (CurrentState.EventTransitions.Contains(EName)) {
			this->UpdateState(CurrentState.EventTransitions[EName]);
		}
	}
}

bool UProcStateMachine::NeedsTick() {
	if (this->CurrentStateName == NAME_None) {
		return false;
	} else {
		return this->GetCurrentState().Node->bNeedsTick;
	}
}

#pragma endregion

#pragma region NodeCode

void UStateNode::Initialize_Implementation(UProcStateMachine* POwner) {
	this->Owner = POwner;
}

void UStateNode::GoTo(FName State) {
	this->Owner->UpdateState(State);
}

AActor* UStateNode::GetOwner() {
	return this->Owner->GetOwner();
}

void UStateNode::Event_Implementation(FName EName) {
	// Does Nothing by default.
}

#pragma endregion