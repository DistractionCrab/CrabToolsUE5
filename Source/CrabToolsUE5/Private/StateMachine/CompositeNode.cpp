// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/CompositeNode.h"

void UCompositeNode::Initialize_Implementation() {
	Super::Initialize_Implementation();
	for (auto& NodePairs : this->Nodes) {
		NodePairs.Value->Initialize_Internal(this->GetMachine());
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
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
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
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

#if WITH_EDITORONLY_DATA
void UCompositeNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);
	
	for (auto Child : this->Nodes)
	{
		Child.Value->GetEmittedEvents(Events);
	}
}
#endif