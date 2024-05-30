#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"


void UEdBaseNode::Inspect()
{
	if (auto Graph = Cast<UEdStateGraph>(this->GetGraph()))
	{
		Graph->Events.OnObjectInspected.Broadcast(this);
	}
}

void UEdBaseStateNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

void UEdBaseStateNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void UEdBaseNode::ClearEvents()
{
	UE_LOG(LogTemp, Warning, TEXT("Clearing events in BaseNode."));
	this->Events.OnNameChanged.Clear();
	this->Events.OnNodeDeleted.Clear();
}