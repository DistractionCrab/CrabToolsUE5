#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/StateMachineBlueprint.h"

#include "StateMachine/ArrayNode.h"

#define LOCTEXT_NAMESPACE "EdStateNode"

UEdStateNode::UEdStateNode() {
	this->bCanRenameNode = true;
}

UEdStateNode::~UEdStateNode() {

}

TSubclassOf<UStateNode> UEdStateNode::GetNodeClass() const {
	return UStateNode::StaticClass();
}

FName UEdStateNode::SetStateName(FName NewName)
{
	if (UEdStateGraph* Graph = Cast<UEdStateGraph>(this->GetGraph()))
	{
		if (Graph->IsStateNameAvilable(NewName))
		{
			const FScopedTransaction Transaction(LOCTEXT("SetStateName", "Set State Name"));
			this->Modify();

			FName OldName = this->StateName;
			this->StateName = NewName;
			this->Events.OnNameChanged.Broadcast(OldName, this->StateName);
		}
	}

	return this->StateName;
}

UStateNode* UEdStateNode::GetCompiledNode()
{
	if (this->Nodes.Num() == 0)
	{
		return NewObject<UStateNode>(this);
	} 
	else if (this->Nodes.Num() == 1)
	{
		return this->Nodes[0];
	}
	else
	{
		auto ArrayNode = NewObject<UArrayNode>(this);

		for (auto Node : this->Nodes)
		{
			ArrayNode->AddNode(DuplicateObject(Node, ArrayNode));
		}

		return ArrayNode;
	}
}

void UEdStateNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

}

TArray<FString> UEdStateNode::GetEventOptions() const
{
	TSet<FName> EventsSet;

	for (auto Node : this->Nodes)
	{
		Node->GetEmittedEvents(EventsSet);
	}

	TArray<FString> EventArray;
	for (auto Event : EventsSet) { EventArray.Add(Event.ToString()); }
	for (auto Event : this->GetStateGraph()->GetEventOptions()) { EventArray.Add(Event); }
	
	return EventArray;
}

void UEdStateNode::Delete()
{
	this->Modify();

	const UEdGraphSchema* Schema = this->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*this);
	}

	this->DestroyNode();
}

bool UEdStateNode::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);

	this->GetGraph()->Modify(bAlwaysMarkDirty);
}

#undef LOCTEXT_NAMESPACE