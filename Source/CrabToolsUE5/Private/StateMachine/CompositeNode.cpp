// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/CompositeNode.h"

void UCompositeNode::Initialize_Implementation(UProcStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);
	for (auto& NodePairs : this->Nodes) {
		NodePairs.Value->Initialize_Internal(POwner);
	}
}

void UCompositeNode::Tick_Implementation(float DeltaTime) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Tick_Internal(DeltaTime);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::Event_Implementation(FName Event) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Event_Internal(Event);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::EventWithData_Implementation(FName Event, UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->EventWithData_Internal(Event, Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::Enter_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Enter_Internal();
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::EnterWithData_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->EnterWithData_Internal(Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::Exit_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Exit_Internal();	
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::ExitWithData_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->ExitWithData_Internal(Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

UStateNode* UCompositeNode::FindNodeByArray_Implementation(const TArray<FString>& Path, ENodeSearchResult& Branches) {
	if (Path.Num() == 0) {
		Branches = ENodeSearchResult::NOTFOUND;
		return nullptr;
	}
	else {
		FName Name(Path.Last());
		if (this->Nodes.Contains(Name)) {
			if (Path.Num() == 1) {
				Branches = ENodeSearchResult::FOUND;
				return this->Nodes[Name];
			}
			else {
				TArray<FString> Tail(Path);
				Tail.Pop();
				return this->Nodes[Name]->FindNodeByArray(Tail, Branches);
			}
		}
		else {
			Branches = ENodeSearchResult::NOTFOUND;
			return nullptr;
		}
	}
}