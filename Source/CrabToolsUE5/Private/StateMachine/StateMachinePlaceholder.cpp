#include "StateMachine/StateMachinePlaceholder.h"

void UStateMachinePlaceholderNode::Initialize_Implementation()
{
	/*
	if (auto SubMachine = this->GetMachine()->GetSharedMachines().Find(this->SlotName))
	{
		this->SubMachine = SubMachine->Get();
	}
	*/
}

void UStateMachinePlaceholderNode::Tick_Implementation(float DeltaTime)
{ 

}

void UStateMachinePlaceholderNode::Event_Implementation(FName Event) 
{ 

}
void UStateMachinePlaceholderNode::EventWithData_Implementation(FName EName, UObject* Data)
{ 

}

void UStateMachinePlaceholderNode::Enter_Implementation()
{ 

}

void UStateMachinePlaceholderNode::EnterWithData_Implementation(UObject* Data)
{ 

}

void UStateMachinePlaceholderNode::Exit_Implementation()
{

}

void UStateMachinePlaceholderNode::ExitWithData_Implementation(UObject* Data)
{

}