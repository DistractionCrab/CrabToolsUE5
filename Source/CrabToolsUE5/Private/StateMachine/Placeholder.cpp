#include "StateMachine/Placeholder.h"

void UPlaceholderNode::Initialize_Implementation()
{
	if (auto Node = this->GetMachine()->GetSharedNodes().Find(this->SlotName))
	{
		this->SubNode = Node->Get();
	}
}

void UPlaceholderNode::Tick_Implementation(float DeltaTime)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Tick_Internal(DeltaTime);
	} 
}

void UPlaceholderNode::Event_Implementation(FName Event)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Event_Internal(Event);
	} 
}

void UPlaceholderNode::EventWithData_Implementation(FName EName, UObject* Data)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->EventWithData_Internal(EName, Data); 
	} 
}

void UPlaceholderNode::Enter_Implementation()
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Enter_Internal();
	} 
}

void UPlaceholderNode::EnterWithData_Implementation(UObject* Data)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->EnterWithData_Internal(Data);
	} 
}

void UPlaceholderNode::Exit_Implementation()
{ 
	if (this->SubNode)
	{ 
		this->SubNode->Exit_Internal();
	}
}

void UPlaceholderNode::ExitWithData_Implementation(UObject* Data)
{ 
	if (this->SubNode)
	{ 
		this->SubNode->ExitWithData_Internal(Data);
	}
}

void UPlaceholderNode::PostTransition_Implementation()
{
	if (this->SubNode)
	{
		this->SubNode->PostTransition_Internal();
	}
}