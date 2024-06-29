#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"

#define DEFAULT_NODE_NAME "NewState"
#define DEFAULT_EVENT_NAME "NewEvent"

UEdStateGraph::UEdStateGraph()
{
	this->SourceClass = UStateMachine::StaticClass();
}

void UEdStateGraph::NotifyGraphChanged(const FEdGraphEditAction& Action)
{

	Super::NotifyGraphChanged(Action);

	if (Action.Action & EEdGraphActionType::GRAPHACTION_AddNode)
	{
		for (auto Node : Action.Nodes)
		{
			UEdStateNode* CastNode = Cast<UEdStateNode>(const_cast<UEdGraphNode*>(Node));

			if (CastNode)
			{
				this->Events.OnStateAdded.Broadcast(CastNode);
			}			
		}
	}

	this->GetBlueprintOwner()->Modify();
}

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

UEdStartStateNode* UEdStateGraph::GetStartNode() const
{
	TArray<UEdStartStateNode*> PossibleStarts;
	this->GetNodesOfClass<UEdStartStateNode>(PossibleStarts);

	// Should never be null.
	return PossibleStarts[0];
}

TArray<UEdBaseNode*> UEdStateGraph::GetDestinations(UEdBaseNode* Node) const
{
	TArray<UEdBaseNode*> Destinations;

	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	for (auto Trans : Transitions)
	{
		if (Trans->GetStartNode() == Node)
		{
			Destinations.Add(Trans->GetEndNode());
		}
	}

	return Destinations;
}

FName UEdStateGraph::GetStartStateName()
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	for (auto Trans : Transitions)
	{
		if (auto Start = Cast<UEdStartStateNode>(Trans->GetStartNode()))
		{
			if (auto Dest = Cast<UEdStateNode>(Trans->GetEndNode()))
			{
				return Dest->GetStateName();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Destination from Start node was not a state node."));
			}
		}
	}

	return NAME_None;
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

TArray<TObjectPtr<UEdStateNode>> UEdStateGraph::GetStates()
{
	TArray<TObjectPtr<UEdStateNode>> StateNodes;

	for (auto Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdStateNode>(Node))
		{
			StateNodes.Add(StateNode);
		}
	}

	return StateNodes;
}

UStateMachine* UEdStateGraph::GenerateStateMachine(UObject* Outer)
{
	auto StateMachine = NewObject<UStateMachine>(Outer);

	for (auto State : this->GetStates())
	{
		StateMachine->AddStateWithNode(State->GetStateName(), State->GetCompiledNode());
	}

	StateMachine->StartState = this->GetStartStateName();

	return StateMachine;
}

bool UEdStateGraph::IsMainGraph()
{
	if (auto BP = this->GetBlueprintOwner())
	{
		return BP->IsMainGraph(this);
	}

	return false;
}

UStateMachineBlueprint* UEdStateGraph::GetBlueprintOwner() const
{
	return Cast<UStateMachineBlueprint>(this->GetOuter());
}

void UEdStateGraph::Select()
{
	this->GetBlueprintOwner()->SelectGraph(this);
}

void UEdStateGraph::Inspect()
{
	this->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
}

TArray<FString> UEdStateGraph::GetStateOptions() const
{
	TArray<FString> Names;

	for (auto Node : this->Nodes)
	{
		if (auto StateNode = Cast<UEdStateNode>(Node))
		{
			Names.Add(StateNode->GetStateName().ToString());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetEventOptions() const
{
	TArray<FString> Names;

	for (auto Ev : this->EventObjects)
	{
		Names.Add(Ev->GetName().ToString());
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetConditionOptions() const
{
	TArray<FString> Names;
	//auto GraphRef = Cast<UEdStateGraph>(this->GetGraph());
	//auto BP = GraphRef->GetBlueprintOwner();
	

	UClass* ClassBase;

	if (this->bIsMainGraph)
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
	}
	else
	{
		ClassBase = this->SourceClass.Get();
	}

	// Find the example function to check against to find others of the same signature.
	auto FnArchetype = ClassBase->FindFunctionByName("TrueCondition");

	for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(FnArchetype))
		{
			Names.Add(f->GetName());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetMachineOptions() const
{
	return this->GetBlueprintOwner()->GetMachineOptions();
}