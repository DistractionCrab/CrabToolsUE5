#include "StateMachine/ProcStateMachine.h"
#include "Logging/StructuredLog.h"
#include "Algo/Reverse.h"

#pragma region StateMachine Code

void UProcStateMachine::Initialize_Implementation(AActor* POwner) {
	this->Owner = POwner;
	this->CurrentStateName = this->StartState;


	for (auto& pair : this->Graph) {
		auto& StateName = pair.Key;
		auto& StateData = pair.Value;		
		
		if (StateData.Node) {
			StateData.Node->Initialize(this);	
		}				
	}

	// Now setup the inverse alias map.
	for (const auto& pair : this->Aliases) {
		for (const auto& StateName : pair.Value.States) {
			auto Data = this->Graph.Find(StateName);
			if (Data == nullptr) {
				// Shouldn't happen unless someone edited the aliases directly outside of the editor.
				UE_LOGFMT(LogTemp, Error, "Alias refers to nonexistent state: Alias: {0}, State: {1}", pair.Key, StateName);
			}
			else {
				for (const auto& Transitions : pair.Value.Transitions) {
					if (Data->EventTransitions.Contains(Transitions.Key)) {
						UE_LOGFMT(
							LogTemp, 
							Error, 
							"Alias overwrites event transition for state: Alias: {0}, State: {1}, Event {2}", 
							pair.Key, 
							StateName,
							Transitions.Key);
					}
					else {
						Data->EventTransitions.Add(Transitions.Key, Transitions.Value);
					}
				}
			}
		}
	}

	this->UpdateState(this->StartState);
	auto CurrentState = this->GetCurrentState();
	if (CurrentState->Node != nullptr) {
		CurrentState->Node->Enter();
	}
}

AActor* UProcStateMachine::GetOwner() {
	return this->Owner;
}

void UProcStateMachine::UpdateState(FName Name) {
	if (this->Graph.Contains(Name) && Name != this->CurrentStateName) {
		auto CurrentState = this->GetCurrentState();
		auto TID = this->TRANSITION.EnterTransition();


		// Alert all listeners, and if one of them changes the state, return.
		for (const auto& Listener: this->StateChangeEvents) {
			Listener.ExecuteIfBound(this->CurrentStateName, Name);
			if (!this->TRANSITION.Valid(TID)) {
				return;
			}
		}

		// Always exit on the node, regardless of further state transitions.
		// If there were any updates to the state prior to this, then Exit will have
		// called already, and this function will have returned;
		if (CurrentState->Node) CurrentState->Node->Exit();

		// Only transition if no other state update has occurred.
		if (this->TRANSITION.Valid(TID)) {

			this->CurrentStateName = Name;
			CurrentState = this->GetCurrentState();
			if (CurrentState->Node) CurrentState->Node->Enter();
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


void UProcStateMachine::Reset() {
	this->CurrentStateName = this->StartState;
}

void UProcStateMachine::Event(FName EName) {
	// Need to validate possible transitions.
	auto TID = this->TRANSITION.EnterTransition();
	auto CurrentState = this->GetCurrentState();

	if (CurrentState) {
		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->EventTransitions.Contains(EName)) {
			this->UpdateState(CurrentState->EventTransitions[EName]);
		}
		// If there were no declarative event transitions, then we check through aliases.
		else {

		}

		// If the current node's event code didn't change the graph's state, then we check for
		// static event transitions.
		if (this->TRANSITION.Valid(TID)) {
			if (CurrentState->Node) CurrentState->Node->Event(EName);	
		}
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

void UProcStateMachine::StateChangeListen(const FStateChangeDispatcher& Callback) {
	this->StateChangeEvents.Add(Callback);

	if (this->CurrentStateName != NAME_None) {
		Callback.ExecuteIfBound(this->CurrentStateName, this->CurrentStateName);
	}
}

void UProcStateMachine::PreEditChange(FProperty* PropertyAboutToChange) {
	Super::PreEditChange(PropertyAboutToChange);
}

void UProcStateMachine::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) {
	
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	if (PropertyName == "Graph_Key") {
		FName OldValue = NAME_None;
		FName NewValue = NAME_None;

		for (const auto& Name : this->StateList) {
			if (!this->Graph.Contains(Name)) {
				OldValue = Name;
				break;
			}
		}

		if (OldValue != NAME_None) {
			for (const auto& Node : this->Graph) {
				if (!this->StateList.Contains(Node.Key)) {
					NewValue = Node.Key;
					break;
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("Found NewValue = %s"), *NewValue.ToString());

			// Convert Alias Names.
			for (auto& AliasNode : this->Aliases) {
				AliasNode.Value.States.Remove(OldValue);
				AliasNode.Value.States.Add(NewValue);

				for (auto& TransNode : AliasNode.Value.Transitions) {
					if (TransNode.Value == OldValue) {
						AliasNode.Value.Transitions.Add(TransNode.Key, NewValue);
					}
				}
			}

			// Update Destination State Names for event transitions in aliases.
		}
	}
	Super::PostEditChangeChainProperty(e);
}

void UProcStateMachine::PostEditChangeProperty(struct FPropertyChangedEvent& e) {
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UProcStateMachine, Aliases)) {
		
	} else if (PropertyName == GET_MEMBER_NAME_CHECKED(UProcStateMachine, Graph)) {
		this->StateList.Reset();
		for (const auto& States : this->Graph) {
			if (!this->StateList.Contains(States.Key)) {
				this->StateList.Add(States.Key);
			}
		}
	}

	
    Super::PostEditChangeProperty(e);
}

FName UProcStateMachine::GetStateName(UStateNode* Node) {
	FName Found = NAME_None;

	for (const auto& Nodes : this->Graph) {
		if (Nodes.Value.Node == Node) {
			Found = Nodes.Key;
			break;
		}
	}

	return Found;
}

UStateNode* UProcStateMachine::FindNodeByPath_Implementation(const FString& Path, ENodeSearchResult& Branches) {
	TArray<FString> PathList;

	Path.ParseIntoArray(PathList, TEXT("/"), true);
	Algo::Reverse(PathList);

	return this->FindNodeByArray(PathList, Branches);
}

UStateNode* UProcStateMachine::FindNodeByArray_Implementation(const TArray<FString>& Path, ENodeSearchResult& Branches) {
	if (Path.Num() == 0) {
		Branches = ENodeSearchResult::NOTFOUND;
		return nullptr;
	}
	else {
		FName Name(Path.Last());
		if (this->Graph.Contains(Name)) {
			if (Path.Num() == 1) {
				Branches = ENodeSearchResult::FOUND;
				return this->Graph[Name].Node;
			}
			else {
				TArray<FString> Tail(Path);
				Tail.Pop();
				return this->Graph[Name].Node->FindNodeByArray(Tail, Branches);
			}			
		}
		else {
			Branches = ENodeSearchResult::NOTFOUND;
			return nullptr;
		}
	}
}

FName UProcStateMachine::GetCurrentStateName() {
	return this->CurrentStateName;
}

#pragma endregion

#pragma region NodeCode

void UStateNode::Initialize_Implementation(UProcStateMachine* POwner) {
	this->Owner = POwner;
}

UProcStateMachine* UStateNode::GetMachine() {
	return this->Owner;
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

void UStateNode::SetOwner(UProcStateMachine* Parent) {
	this->Owner = Parent;
}

UStateNode* UStateNode::FindNodeByPath_Implementation(const FString& Path, ENodeSearchResult& Branches) {
	TArray<FString> PathList;
	Path.ParseIntoArray(PathList, TEXT("/"), true);
	return this->FindNodeByArray(PathList, Branches);
}

UStateNode* UStateNode::FindNodeByArray_Implementation(const TArray<FString>& Path, ENodeSearchResult& Branches) {
	return nullptr;
}

FName UStateNode::GetStateName() {
	FName Found = NAME_None;

	if (this->Owner) {
		return this->Owner->GetStateName(this);
	}
	else {
		return NAME_None;
	}
	
}

#pragma endregion