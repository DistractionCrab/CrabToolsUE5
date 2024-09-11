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

FName UEdStateNode::GetStateName() const
{ 
	if (this->StateClass->GetAccess() == EStateMachineAccessibility::PRIVATE)
	{
		FString Total;
		Total.Append(this->GetStateGraph()->GetClassPrefix().ToString());
		Total.Append("::");
		Total.Append(this->StateName.ToString());

		return FName(Total);
	}
	else
	{
		return this->StateName;
	}
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

UState* UEdStateNode::GenerateState(FNodeVerificationContext& Context, UObject* Outer)
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