#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/Utils.h"

#include "StateMachine/ArrayNode.h"

#define LOCTEXT_NAMESPACE "EdStateNode"

UEdStateNode::UEdStateNode() {
	this->bCanRenameNode = true;

	this->StateClass = CreateDefaultSubobject<UState>(TEXT("DefaultStateClass"));
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

UState* UEdStateNode::GetCompiledState(UObject* Outer)
{
	if (!IsValid(this->StateClass.Get()))
	{
		this->Modify();
		this->StateClass = NewObject<UState>(this);
	}

	UState* BuiltState = DuplicateObject(this->StateClass, Outer, this->GetStateName());	

	if (this->Nodes.Num() == 1)
	{
		BuiltState->AppendNodeCopy(this->Nodes[0]);
	}
	else
	{
		auto ArrayNode = NewObject<UArrayNode>(BuiltState);

		for (auto Node : this->Nodes)
		{
			ArrayNode->AddNode(DuplicateObject(
				Node, 
				ArrayNode, 
				Naming::GenerateStateNodeName(Node, this->GetStateName())));
		}

		BuiltState->AppendNode(ArrayNode);
	}

	return BuiltState;
}



TArray<FString> UEdStateNode::GetEventOptions() const
{
	TSet<FName> EventsSet;

	for (auto Node : this->Nodes)
	{
		if (IsValid(Node))
		{
			Node->GetEmittedEvents(EventsSet);
		}
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
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

bool UEdStateNode::HasEvent(FName EName)
{
	return this->NodeEmittedEvents.Contains(EName) || this->GetStateGraph()->HasEvent(EName);
}

void UEdStateNode::UpdateEmittedEvents()
{
	this->NodeEmittedEvents.Empty();

	for (auto Node : this->Nodes)
	{
		if (Node)
		{
			TSet<FName> PartEvents;

			Node->GetEmittedEvents(PartEvents);

			this->NodeEmittedEvents.Append(PartEvents);
		}
	}
}

TArray<FString> UEdStateNode::GetEnterStates() const
{
	TArray<FString> Names;
	
	for (auto Pin : this->Pins)
	{
		if (Pin->LinkedTo.Num() > 0)
		{
			for (auto LinkedTo : Pin->LinkedTo)
			{
				if (auto CastNode = Cast<UEdTransition>(LinkedTo->GetOwningNode()))
				{
					if (CastNode->GetEndNode() == this)
					{
						if (auto CastStateNode = Cast<UEdStateNode>(CastNode->GetStartNode()))
						{
							Names.Add(CastStateNode->GetStateName().ToString());
						}
					}
				}
			}
		}
	}	

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateNode::GetExitStates() const
{
	TArray<FString> Names;

	for (auto Pin : this->Pins)
	{
		if (Pin->LinkedTo.Num() > 0)
		{
			for (auto LinkedTo : Pin->LinkedTo)
			{
				if (auto CastNode = Cast<UEdTransition>(LinkedTo->GetOwningNode()))
				{
					if (CastNode->GetStartNode() == this)
					{
						if (auto CastStateNode = Cast<UEdStateNode>(CastNode->GetEndNode()))
						{
							Names.Add(CastStateNode->GetStateName().ToString());
						}
					}
				}
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateNode::GetStateClassesOptions() const
{
	return this->GetStateGraph()->GetBlueprintOwner()->GetStateClassesOptions();
}

#if WITH_EDITOR

void UEdStateNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	this->UpdateEmittedEvents();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateNode, Nodes))
	{
		this->Modify();
	}

	if (!IsValid(this->StateClass))
	{
		this->StateClass = NewObject<UState>(this);
	}
}

void UEdStateNode::PostLinkerChange()
{
	this->UpdateEmittedEvents();
}
#endif

#undef LOCTEXT_NAMESPACE