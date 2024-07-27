#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"

#include "StateMachine/ArrayNode.h"


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
			this->StateName = NewName;
			this->Events.OnNameChanged.Broadcast(this->StateName);
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
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UE_LOG(LogTemp, Warning, TEXT("PostEdit"));

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.MemberProperty)
		{
			UE_LOG(LogTemp, Warning, TEXT("PostEdit: Member Property was not null."));
			UE_LOG(LogTemp, Warning, TEXT("PostEdit: Found a class: %s"),
				*PropertyChangedEvent.MemberProperty->GetName());
		}		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PostEdit: Property was null."));
	}
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

}