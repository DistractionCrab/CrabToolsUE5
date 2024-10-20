#include "StateMachine/CompositeNode.h"

void UCompositeNode::Initialize_Inner_Implementation() {
	Super::Initialize_Inner_Implementation();
	for (auto& NodePairs : this->Nodes) {
		NodePairs.Value->Initialize(this->GetMachine());
	}
}

void UCompositeNode::Tick_Inner_Implementation(float DeltaTime) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Tick(DeltaTime);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::Event_Inner_Implementation(FName Event) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Event(Event);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::EventWithData_Inner_Implementation(FName Event, UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->EventWithData(Event, Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::Enter_Inner_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Enter();
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::EnterWithData_Inner_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->EnterWithData(Data);
			if (!(this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::Exit_Inner_Implementation() {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Exit();	
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

void UCompositeNode::ExitWithData_Inner_Implementation(UObject* Data) {
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->ExitWithData(Data);
			if (!(!this->Active() && this->GetMachine()->IsInState(TID))) {
				return;
			}
		}
	}
}

bool UCompositeNode::RequiresTick_Implementation() const
{ 
	for (const auto& Node : this->Nodes)
	{
		if (IsValid(Node.Value) && Node.Value->RequiresTick())
		{
			return true;
		}
	}

	return false;
}

void UCompositeNode::PostTransition_Inner_Implementation()
{
	int TID = this->GetMachine()->GetStateID();
	for (const auto& Node : this->Nodes)
	{
		if (Node.Value)
		{
			Node.Value->PostTransition();
			if (!(!this->Active() && this->GetMachine()->IsInState(TID)))
			{
				return;
			}
		}
	}
}

bool UCompositeNode::HasPipedData_Implementation() const
{
	for (const auto& Child : this->Nodes)
	{
		if (Child.Value->HasPipedData())
		{
			return true;
		}
	}

	return false;
}

UObject* UCompositeNode::GetPipedData_Implementation()
{
	auto Data = NewObject<UCompositeNodeData>(this);

	for (auto& Child : this->Nodes)
	{
		if (Child.Value->HasPipedData())
		{
			Data->AddData(Child.Key, Child.Value->GetPipedData());
		}
	}

	return Data;
}

void UCompositeNodeData::AddData(FName Key, UObject* AddedData)
{
	if (IsValid(AddedData))
	{
		this->Data.Add(Key, AddedData);
	}
}

UObject* UCompositeNodeData::FindDataOfType_Implementation(TSubclassOf<UObject> Type)
{
	for (auto& Child : this->Data)
	{
		if (auto Check = UStateMachineDataHelpers::FindDataOfType(Type, Child.Value))
		{
			return Check;
		}
	}

	return nullptr;
}

void UCompositeNodeData::FindAllDataOfType_Implementation(TSubclassOf<UObject> Type, TArray<UObject*>& ReturnValue)
{
	for (auto& Child : this->Data)
	{
		if (Child.Value->IsA(Type))
		{
			ReturnValue.Add(Child.Value);
		}

		UStateMachineDataHelpers::FindAllDataOfType(Type, Child.Value, ReturnValue);
	}
}

TScriptInterface<UInterface> UCompositeNodeData::FindDataImplementing_Implementation(TSubclassOf<UInterface> Type)
{
	for (auto& Child : this->Data)
	{
		if (Child.Value->GetClass()->ImplementsInterface(Type))
		{
			TScriptInterface<UInterface> T(Child.Value);

			return T;
		}
	}

	return nullptr;
}

void UCompositeNodeData::FindAllDataImplementing_Implementation(TSubclassOf<UInterface> Type, TArray<TScriptInterface<UInterface>>& ReturnValue)
{
	for (auto& Child : this->Data)
	{
		if (Child.Value->GetClass()->ImplementsInterface(Type))
		{
			TScriptInterface<UInterface> T(Child.Value);

			ReturnValue.Add(T);
		}

		UStateMachineDataHelpers::FindAllDataImplementing(Type, Child.Value, ReturnValue);
	}
}

#if WITH_EDITORONLY_DATA
void UCompositeNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);
	
	for (auto& Child : this->Nodes)
	{
		Child.Value->GetEmittedEvents(Events);
	}
}
#endif