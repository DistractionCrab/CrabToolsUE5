#include "StateMachine/EdGraph/EdStateExtensionNode.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/Utils.h"

#include "StateMachine/ArrayNode.h"

#define LOCTEXT_NAMESPACE "EdStateNode"

UEdStateExtensionNode::UEdStateExtensionNode() {
	this->bCanRenameNode = true;

	this->StateClass = CreateDefaultSubobject<UState>(TEXT("DefaultStateClass"));
}

UEdStateExtensionNode::~UEdStateExtensionNode() {

}

TSubclassOf<UStateNode> UEdStateExtensionNode::GetNodeClass() const {
	return UStateNode::StaticClass();
}

FName UEdStateExtensionNode::GetStateName() const
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

UState* UEdStateExtensionNode::GenerateState(FNodeVerificationContext& Context, UObject* Outer)
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

	if (BuiltState->GetNode())
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Node for compiled state %s, Node=%s"), *this->GetStateName().ToString(), *BuiltState->GetNode()->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Node for compiled state %s, Node=null"), *this->GetStateName().ToString());
	}

	return BuiltState;
}



TArray<FString> UEdStateExtensionNode::GetEventOptions() const
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

void UEdStateExtensionNode::Delete()
{
	this->Modify();

	const UEdGraphSchema* Schema = this->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*this);
	}

	this->DestroyNode();
}

bool UEdStateExtensionNode::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

bool UEdStateExtensionNode::HasEvent(FName EName)
{
	return this->NodeEmittedEvents.Contains(EName) || this->GetStateGraph()->HasEvent(EName);
}

void UEdStateExtensionNode::UpdateEmittedEvents()
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

TArray<FString> UEdStateExtensionNode::GetEnterStates() const
{
	TArray<FString> Names = UEdBaseStateNode::GetEnterStates();

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateExtensionNode::GetExitStates() const
{
	TArray<FString> Names = UEdBaseStateNode::GetExitStates();


	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

bool UEdStateExtensionNode::IsOverride() const
{
	return this->bOverrideParent;
}

#if WITH_EDITOR

void UEdStateExtensionNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	this->UpdateEmittedEvents();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateExtensionNode, Nodes))
	{
		this->Modify();
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateExtensionNode, StateName))
	{

	}


	if (!IsValid(this->StateClass))
	{
		this->StateClass = NewObject<UState>(this);
	}
}

void UEdStateExtensionNode::PostLinkerChange()
{
	this->UpdateEmittedEvents();
}

TArray<FString> UEdStateExtensionNode::GetExtendibleStates() const
{
	TSet<FString> Names;

	this->GetStateGraph()->CollectExtendibleStates(Names);

	return Names.Array();
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE