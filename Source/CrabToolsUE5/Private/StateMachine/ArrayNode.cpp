#include "StateMachine/ArrayNode.h"

void UArrayNode::Initialize_Implementation() {
	Super::Initialize_Implementation();
	for (auto& Node : this->Nodes) {
		if (IsValid(Node))
		{
			Node->Initialize_Internal(this->GetMachine());
		}
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

void UArrayNode::PostTransition_Implementation()
{
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes)
	{
		if (Node)
		{
			Node->PostTransition_Internal();
			if (!(!this->Active() && this->GetMachine()->IsInState(TID)))
			{
				return;
			}
		}
	}
}

bool UArrayNode::RequiresTick_Implementation() const
{
	for (const auto& Node : this->Nodes)
	{
		if (IsValid(Node) && Node->RequiresTick())
		{
			return true;
		}
	}

	return false;
}

#if WITH_EDITORONLY_DATA
void UArrayNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);

	for (auto Child : this->Nodes)
	{
		Child->GetEmittedEvents(Events);
	}
}
#endif