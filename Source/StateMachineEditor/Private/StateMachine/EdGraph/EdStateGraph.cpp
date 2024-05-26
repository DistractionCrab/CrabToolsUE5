#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdEventObject.h"

#define DEFAULT_NODE_NAME "NewState"
#define DEFAULT_EVENT_NAME "NewEvent"

FName UEdStateGraph::GetNewStateName()
{
	FString DefaultName(DEFAULT_NODE_NAME);
	int index = 0;

	for (auto Node : this->Nodes)
	{
		if (UEdStateNode* CastNode = Cast<UEdStateNode>(Node))
		{
			FString NodeName = CastNode->GetStateName().ToString();

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

bool UEdStateGraph::IsStateNameAvilable(FName Name) const
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


void UEdStateGraph::ClearDelegates()
{
	this->Events.OnNodeSelected.Clear();
	this->Events.OnEventCreated.Clear();

	for (auto Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdBaseStateNode>(Node))
		{
			StateNode->ClearEvents();
		}
	}
}

UEdEventObject* UEdStateGraph::CreateEvent()
{
	FString DefaultName(DEFAULT_EVENT_NAME);
	int index = 0;

	for (auto Event : this->EventObjects)
	{
		FString EventName = Event->GetName().ToString();

		if (EventName.StartsWith(DEFAULT_EVENT_NAME))
		{
			FString EndName = EventName.RightChop(DefaultName.Len());

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

	DefaultName.AppendInt(index);

	auto NewName = FName(DefaultName);
	auto NewEventObject = NewObject<UEdEventObject>(this);
	NewEventObject->SetName(NewName);

	this->EventObjects.Add(NewEventObject);

	this->Events.OnEventCreated.Broadcast(NewEventObject);

	return NewEventObject;
}

bool UEdStateGraph::IsEventNameAvilable(FName Name) const
{
	for (auto EventObj : this->EventObjects)
	{
		if (EventObj->GetName() == Name)
		{
			return false;
		}
	}

	return true;
}

FName UEdStateGraph::RenameEvent(UEdEventObject* EventObj, FName To)
{
	if (this->IsEventNameAvilable(To))
	{
		EventObj->SetName(To);
		return To;
	}
	else
	{
		return EventObj->GetName();
	}
}