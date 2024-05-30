#include "StateMachine/EdGraph/EdEventObject.h"
#include "STateMachine/EdGraph/EdStateGraph.h"

void UEdEventObject::Inspect()
{
	if (auto Graph = this->GetGraph())
	{
		Graph->Events.OnObjectInspected.Broadcast(this);
	}
}

FName UEdEventObject::RenameEvent(FName NewName)
{
	return this->GetGraph()->RenameEvent(this, NewName);
}

void UEdEventObject::SetName(FName Name)
{
	this->EventName = Name;
	this->Events.OnEventRenamed.Broadcast(Name);
}

UEdStateGraph* UEdEventObject::GetGraph() const
{
	auto Graph = Cast<UEdStateGraph>(GetOuter());

	if (Graph == nullptr && IsValid(this))
	{
		if (GetOuter() != GetTransientPackage())
		{
			ensureMsgf(false, TEXT("EdEventObject::GetGraph : '%s' does not have a UEdStateGraph as an Outer."), *GetPathName());
		}
	}
	return Graph;
}