#include "StateMachine/ArrayNode.h"

void UArrayNode::Initialize_Implementation(UStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);
	for (auto& Node : this->Nodes) {
		Node->Initialize_Internal(POwner);
	}
}

void UArrayNode::Tick_Implementation(float DeltaTime) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Tick_Internal(DeltaTime);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::Event_Implementation(FName Event) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Event_Internal(Event);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::EventWithData_Implementation(FName Event, UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->EventWithData_Internal(Event, Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::Enter_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Enter_Internal();
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::EnterWithData_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->EnterWithData_Internal(Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::Exit_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Exit_Internal();	
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UArrayNode::ExitWithData_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->ExitWithData_Internal(Data);
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

UStateNode* UArrayNode::FindNodeByArray_Implementation(const TArray<FString>& Path, ESearchResult& Branches) {
	return this;
}

UStateNode* UArrayNode::Substitute(FName SlotName, UStateNode* Node) {
	return this;
}

UStateNode* UArrayNode::ExtractAs(TSubclassOf<UStateNode> Class) {
	auto Check = Super::ExtractAs(Class);

	if (!Check) {
		for (auto Node : this->Nodes) {
			if (Node) {
				Check = Node->ExtractAs(Class);

				if (Check) {
					break;
				}
			}			
		}
	}

	return Check;
}