#include "StateMachine/Placeholder.h"

void UPlaceholderNode::Initialize_Inner_Implementation()
{
	if (auto Node = this->GetMachine()->GetSharedNodes().Find(this->SlotName))
	{
		this->SubNode = Node->Get();
	}
}

void UPlaceholderNode::Tick_Inner_Implementation(float DeltaTime)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Tick(DeltaTime);
	} 
}

void UPlaceholderNode::Event_Inner_Implementation(FName Event)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Event(Event);
	} 
}

void UPlaceholderNode::EventWithData_Inner_Implementation(FName EName, UObject* Data)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->EventWithData(EName, Data); 
	} 
}

void UPlaceholderNode::Enter_Inner_Implementation()
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Enter();
	} 
}

void UPlaceholderNode::EnterWithData_Inner_Implementation(UObject* Data)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->EnterWithData(Data);
	} 
}

void UPlaceholderNode::Exit_Inner_Implementation()
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Exit();
	}
}

void UPlaceholderNode::ExitWithData_Inner_Implementation(UObject* Data)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->ExitWithData(Data);
	}
}

void UPlaceholderNode::PostTransition_Inner_Implementation()
{
	if (this->SubNode)
	{
		this->SubNode->PostTransition();
	}
}