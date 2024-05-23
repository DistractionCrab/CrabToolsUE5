#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdStateNode.h"

#define DEFAULT_NODE_NAME "NewState"

FName UEdStateGraph::GetNewStateName()
{
	FString DefaultName(DEFAULT_NODE_NAME);
	int index = 0;

	for (auto Node : this->Nodes)
	{
		if (UEdStateNode* CastNode = Cast<UEdStateNode>(Node))
		{
			FString NodeName = CastNode->GetStateName().ToString();

			UE_LOG(LogTemp, Warning, TEXT("Found Node with name: %s"), *NodeName);

			if (NodeName.StartsWith(DEFAULT_NODE_NAME))
			{
				FString EndName = NodeName.RightChop(DefaultName.Len());

				if (EndName.IsNumeric())
				{
					int check = FCString::Atoi(*EndName);

					if (check >= index)
					{
						index = check + 1;
					}
				}
			}
		}
	}

	DefaultName.AppendInt(index);

	return FName(DefaultName);
}

bool UEdStateGraph::IsNameAvilable(FName Name) const
{
	for (const auto& Node : this->Nodes)
	{
		if (UEdStateNode* CastNode = Cast<UEdStateNode>(Node))
		{
			if (CastNode->GetStateName() == Name)
			{
				return false;
			}
		}
	}

	return true;
}


void UEdStateGraph::ClearEvents()
{
	this->Events.OnNodeSelected.Clear();

	for (auto Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdBaseNode>(Node))
		{
			StateNode->ClearEvents();
		}
	}
}