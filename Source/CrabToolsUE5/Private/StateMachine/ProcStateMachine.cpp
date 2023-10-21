#include "StateMachine/ProcStateMachine.h"
#include "Logging/StructuredLog.h"

#pragma region StateMachine Code

void UProcStateMachine::Initialize_Implementation(AActor* POwner) {
	this->Owner = POwner;
	this->CurrentStateName = this->StartState;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Initializing the State Machine!"));
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
		
		if (StateData.Node) {
			StateData.Node->Initialize(this);	
		} else {
			UE_LOGFMT(LogCore, Error, "Attempted to Initialize on Null Node in State {0}", StateName);
		}
		
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
			CurrentState->Node->Exit();
			this->CurrentStateName = Name;
			CurrentState->Node->Enter();
		}
	}
}

void UProcStateMachine::Tick(float DeltaTime) {
	if (this->CurrentStateName != NAME_None) {
		//if (GEngine)
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ticking in the machine!"));
		auto State = this->GetCurrentState();
		if (State && State->Node) {
			State->Node->Tick(DeltaTime);	
		}		
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

	if (CurrentState) {
		CurrentState->Node->Event(EName);

		// If the current node's event code didn't change the graph's state, then we check for
		// static event transitions.
		if (this->TRANSITION.Valid(TID)) {
			if (CurrentState->EventTransitions.Contains(EName)) {
				this->UpdateState(CurrentState->EventTransitions[EName]);
			}
		}
	}
	else {
		UE_LOGFMT(LogCore, Error, "Attempted to Call Event on Null Node in State {0}", this->CurrentStateName);
	}
	
}

UStateNode* UProcStateMachine::FindNode(FName NodeName, ENodeSearchResult& Branches) {
	if (this->Graph.Contains(NodeName)) {		
		auto Node = this->Graph[NodeName].Node;
		if (Node) {
			Branches = ENodeSearchResult::FOUND;
		} else {
			Branches = ENodeSearchResult::NOTFOUND;
		}
		return Node;
	}
	else {
		Branches = ENodeSearchResult::NOTFOUND;
		return nullptr;
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