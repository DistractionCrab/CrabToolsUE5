#include "StateMachine/ProcStateMachine.h"
#include "Logging/StructuredLog.h"
#include "Algo/Reverse.h"

#pragma region StateMachine Code

void UProcStateMachine::Initialize_Internal(AActor* POwner) {
	this->Owner = POwner;
	//this->CurrentStateName = this->StartState;
	this->Initialize(POwner);

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
					if (Data->Transitions.Contains(Transitions.Key)) {
						UE_LOGFMT(
							LogTemp,
							Error,
							"Alias overwrites event transition for state: Alias: {0}, State: {1}, Event {2}",
							pair.Key,
							StateName,
							Transitions.Key);
					}
					else {
						Data->Transitions.Add(Transitions.Key, Transitions.Value);
						//Data->EventTransitions.Add(Transitions.Key, Transitions.Value);
					}
				}
			}
		}
	}

	for (auto& pair : this->Graph) {
		auto& StateName = pair.Key;
		auto& StateData = pair.Value;

		if (StateData.Node) {
			StateData.Node->Initialize_Internal(this);
		}
	}

	this->RebindConditions();
	this->UpdateState(this->StartState);
}

void UProcStateMachine::Initialize_Implementation(AActor* POwner) {
	
}

AActor* UProcStateMachine::GetOwner() {
	return this->Owner;
}

void UProcStateMachine::UpdateState(FName Name) {
	if (this->Graph.Contains(Name) && Name != this->CurrentStateName) {
		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		auto TID = this->TRANSITION.EnterTransition();
		// Always exit on the node, regardless of further state transitions.
		// If there were any updates to the state prior to this, then Exit will have
		// called already, and this function will have returned;
		if (CurrentState && CurrentState->Node) CurrentState->Node->Exit_Internal();

		if (this->TRANSITION.Valid(TID)) { 
			this->CurrentStateName = Name;
			CurrentState = this->GetCurrentState();
			if (CurrentState->Node) CurrentState->Node->Enter_Internal();

			
			if (this->TRANSITION.Valid(TID)) {
				// Alert all listeners, and if one of them changes the state, return.
				for (const auto& Listener : this->StateChangeEvents) {
					Listener.ExecuteIfBound(OldState, Name);
					if (!this->TRANSITION.Valid(TID)) {
						return;
					}
				}
			}
		}	
	}
}

void UProcStateMachine::UpdateStateWithData(FName Name, UObject* Data) {
	if (this->Graph.Contains(Name) && Name != this->CurrentStateName) {
		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		auto TID = this->TRANSITION.EnterTransition();
		// Always exit on the node, regardless of further state transitions.
		// If there were any updates to the state prior to this, then Exit will have
		// called already, and this function will have returned;
		if (CurrentState && CurrentState->Node) CurrentState->Node->ExitWithData_Internal(Data);

		if (this->TRANSITION.Valid(TID)) {
			this->CurrentStateName = Name;
			CurrentState = this->GetCurrentState();
			if (CurrentState->Node) CurrentState->Node->EnterWithData_Internal(Data);


			if (this->TRANSITION.Valid(TID)) {
				// Alert all listeners, and if one of them changes the state, return.
				for (const auto& Listener : this->StateChangeEvents) {
					Listener.ExecuteIfBound(OldState, Name);
					if (!this->TRANSITION.Valid(TID)) {
						return;
					}
				}
			}
		}
	}
}

void UProcStateMachine::Tick(float DeltaTime) {
	if (this->CurrentStateName != NAME_None) {
		auto State = this->GetCurrentState();
		if (State && State->Node) {
			State->Node->Tick(DeltaTime);	
		}		
	}
}


void UProcStateMachine::Reset() {
	this->UpdateState(this->StartState);
}

void UProcStateMachine::Event(FName EName) {
	// Need to validate possible transitions.
	auto TID = this->TRANSITION.CurrentID();
	auto CurrentState = this->GetCurrentState();

	if (CurrentState) {
		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->Transitions.Contains(EName)) {
			auto TData = CurrentState->Transitions[EName];
			if (TData.ConditionCallback.IsBound()) {
				if (TData.ConditionCallback.Execute()) {
					this->UpdateState(TData.Destination);
				}
			}
			else {
				UE_LOGFMT(LogTemp, Error, "Condition Delegate Unbound? Condition Name: {0}", TData.Condition);
			}
		}
		

		// If the current node's event code didn't change the graph's state, then we check for
		// static event transitions.
		if (this->TRANSITION.Valid(TID)) {
			if (CurrentState->Node) CurrentState->Node->Event_Internal(EName);	
		}
	}	
}


void UProcStateMachine::EventWithData(FName EName, UObject* Data) {
	// Need to validate possible transitions.
	auto TID = this->TRANSITION.CurrentID();
	auto CurrentState = this->GetCurrentState();

	if (CurrentState) {
		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->Transitions.Contains(EName)) {
			auto TData = CurrentState->Transitions[EName];
			if (TData.DataConditionCallback.IsBound()) {
				if (TData.DataConditionCallback.Execute(Data)) {
					this->UpdateStateWithData(TData.Destination, Data);
				}
			}
			else {
				UE_LOGFMT(LogTemp, Error, "Condition Delegate Unbound? Condition Name: {0}", TData.Condition);
			}
		}


		// If the current node's event code didn't change the graph's state, then we check for
		// static event transitions.
		if (this->TRANSITION.Valid(TID)) {
			if (CurrentState->Node) CurrentState->Node->EventWithData_Internal(EName, Data);
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

		// First find the old value of what was changed.
		for (const auto& Name : this->StateList) {
			if (!this->Graph.Contains(Name)) {
				OldValue = Name;
				break;
			}
		}

		// If an old value was found, save it and continue.
		if (OldValue != NAME_None) {
			// Find the value the old value was changed to and save it.
			for (const auto& Node : this->Graph) {
				if (!this->StateList.Contains(Node.Key)) {
					NewValue = Node.Key;
					break;
				}
			}

			// Loop through aliases and remap state names in both what aliases refer to and what their
			// transition destinations refer to.
			for (auto& AliasNode : this->Aliases) {
				AliasNode.Value.States.Remove(OldValue);
				AliasNode.Value.States.Add(NewValue);

				for (auto& TransNode : AliasNode.Value.Transitions) {
					if (TransNode.Value.Destination == OldValue) {
						TransNode.Value.Destination = NewValue;
					}
				}
			}
		}
	}
	Super::PostEditChangeChainProperty(e);
}

/* Simply iterates through the graph and rebinds condition callbacks. */
void UProcStateMachine::RebindConditions() {
	TArray<FString> ValidFunctions = this->ConditionOptions();
	
	for (auto& pairs : this->Graph) {
		for (auto& tpairs : pairs.Value.Transitions) {
			if (ValidFunctions.Contains(tpairs.Value.Condition.ToString())) {
				tpairs.Value.ConditionCallback.BindUFunction(this, tpairs.Value.Condition);
				tpairs.Value.DataConditionCallback.BindUFunction(this, tpairs.Value.DataCondition);
			}
			
		}
	}
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

TArray<FString> UProcStateMachine::StateOptions() {
	TArray<FString> Names;
	for (const auto& Nodes : this->Graph) {
		Names.Add(Nodes.Key.ToString());
	}
	return Names;
}

TArray<FString> UProcStateMachine::ConditionOptions() {
	TArray<FString> Names;
	auto base = this->FindFunction("TrueCondition");
	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;
		
		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}		
	}
	return Names;
}

TArray<FString> UProcStateMachine::ConditionDataOptions() {
	TArray<FString> Names;
	auto base = this->FindFunction("TrueDataCondition");
	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;		
		
		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}
	}
	return Names;
}

bool UProcStateMachine::TrueCondition() { 
	return true; 
}

bool UProcStateMachine::FalseCondition() {
	return false;
}

bool UProcStateMachine::TrueDataCondition(UObject* Data) {
	return true;
}

bool UProcStateMachine::FalseDataCondition(UObject* Data) {
	return false;
}

bool UProcStateMachine::ValidDataCondition(UObject* Data) {
	return Data != nullptr;
}

#pragma endregion

#pragma region NodeCode

void UStateNode::Initialize_Internal(UProcStateMachine* POwner) {
	this->Owner = POwner;
	this->Initialize(POwner);
}

void UStateNode::Initialize_Implementation(UProcStateMachine* POwner) {
	
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

void UStateNode::Event_Internal(FName EName) {
	if (this->bActive) {
		this->Event(EName);
	}
}

void UStateNode::Event_Implementation(FName EName) {
	// Does Nothing by default.
}

void UStateNode::EventWithData_Internal(FName EName, UObject* Data) {
	if (this->bActive) {
		this->EventWithData(EName, Data);
	}
}

void UStateNode::EventWithData_Implementation(FName EName, UObject* Data) {
	this->Event(EName);
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

void UStateNode::Tick_Internal(float DeltaTime) {
	if (this->bActive) {
		this->Tick(DeltaTime);
	}
}

void UStateNode::Exit_Internal() {
	if (this->bActive) {
		this->bActive = false;
		this->Exit();
	}	
}

void UStateNode::Enter_Internal() {
	if (!this->bActive) {
		this->bActive = true;
		this->Enter();
	}
}

void UStateNode::EnterWithData_Internal(UObject* Data) {
	if (!this->bActive) {
		this->bActive = true;
		this->EnterWithData(Data);
	}
}

void UStateNode::EnterWithData_Implementation(UObject* Data) {
	this->Enter();
}

void UStateNode::ExitWithData_Internal(UObject* Data) {
	if (!this->bActive) {
		this->bActive = true;
		this->ExitWithData(Data);
	}
}

void UStateNode::ExitWithData_Implementation(UObject* Data) {
	this->Exit();
}

#pragma endregion