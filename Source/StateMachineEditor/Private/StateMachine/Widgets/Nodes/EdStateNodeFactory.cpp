#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"

#include "StateMachine/Widgets/Nodes/SEdStateNode.h"
#include "StateMachine/Widgets/Nodes/SEdStartStateNode.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include <EdGraph/EdGraphNode.h>


TSharedPtr<class SGraphNode> FEdStateNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (auto StateNode = Cast<UEdStateNode>(Node))
	{
		return SNew(SEdStateNode, StateNode);
	} 
	else if (auto StartNode = Cast<UEdStartStateNode>(Node)) 
	{
		return SNew(SEdStartStateNode, StartNode);
	}
	
	return nullptr;
}

