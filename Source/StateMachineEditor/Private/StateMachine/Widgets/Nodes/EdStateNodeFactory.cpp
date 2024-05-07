#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"
#include "StateMachine/Widgets/Nodes/EdStateNode.h"

#include <EdGraph/EdGraphNode.h>


TSharedPtr<class SGraphNode> FEdStateNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (UEdStateNode* EdNode_GraphNode = Cast<UEdStateNode>(Node))
	{
		return SNew(SEdStateNode, EdNode_GraphNode);
	}
	
	return nullptr;
}

TSubclassOf<UStateNode> UEdStateNode::GetNodeClass() const {
	return UStateNode::StaticClass();
}