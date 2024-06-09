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
	if (this->Nodes.Num() == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Returning simple node for compile."));
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