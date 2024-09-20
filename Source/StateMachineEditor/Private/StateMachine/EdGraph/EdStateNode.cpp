#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/Utils.h"
#include "Utils/PropertyManagement.h"

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
	FName OldName = this->StateName;
	this->StateName = NewName;
	this->Events.OnNameChanged.Broadcast(OldName, NewName);

	return NewName;
}

void UEdStateNode::RenameNode(FName Name)
{
	this->GetStateGraph()->RenameNode(this, Name);
}

UState* UEdStateNode::GenerateState(FNodeVerificationContext& Context, UObject* Outer)
{
	if (!IsValid(this->StateClass.Get()))
	{
		this->Modify();
		this->StateClass = NewObject<UState>(this);
	}
	UState* BuiltState = nullptr;

	if (this->NodeType == EStateNodeType::INLINE_NODE)
	{
		BuiltState = DuplicateObject(this->StateClass, Outer, this->GetStateName());
	}
	else if (this->NodeType == EStateNodeType::EXTENDED_NODE)
	{
		BuiltState = NewObject<UState>(Outer, this->GetStateName());
	}
	else
	{
		BuiltState = DuplicateObject(this->StateClassOverride.Value, Outer, this->GetStateName());
	}

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

TArray<FString> UEdStateNode::GetInheritableStates() const
{
	TArray<FString> Names = this->GetStateGraph()->GetInheritableStates(this->NodeType);

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

bool UEdStateNode::UpdateStateArchetypeOverride()
{
	bool bOverrideWasModified = false;

	if (this->NodeType == EStateNodeType::EXTENDED_NODE 
		|| this->NodeType == EStateNodeType::OVERRIDE_EXTENDED_NODE)
	{
		if (this->StateExtension.IsNone())
		{
			this->Accessibility = EStateMachineAccessibility::PRIVATE;
			this->StateClassOverride.Value = nullptr;
		}
		else
		{
			if (this->GetFName() != this->StateExtension)
			{
				this->RenameNode(this->StateExtension);
			}

			if (!IsValid(this->StateClassOverride.DefaultObject))
			{
				if (auto BPObj = this->GetStateGraph()->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						auto DuplicatedState = Class->DuplicateStateArchetype(this->GetStateGraph()->GetFName(), this->StateExtension, this);
						this->Accessibility = DuplicatedState->GetAccess();
						this->StateClassOverride.DefaultObject = DuplicatedState;
						this->StateClassOverride.Value = DuplicateObject(DuplicatedState, this);
					}
				}
			}
			else
			{
				if (auto BPObj = this->GetStateGraph()->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						bOverrideWasModified = UPropertyManagementLibrary::UpdateObjectInheritanceProperties<UState>(
							Class->GetStateArchetype(this->GetStateGraph()->GetFName(), this->StateExtension),
							this->StateClassOverride.DefaultObject,
							this->StateClassOverride.Value);
					}
				}
			}
		}
	}
	else
	{
		this->StateExtension = NAME_None;
		this->StateClassOverride.Value = nullptr;
		this->StateClassOverride.DefaultObject = nullptr;
	}

	return bOverrideWasModified;
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
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateNode, NodeType))
	{
		if (this->NodeType == EStateNodeType::INLINE_NODE)
		{
			this->StateExtension = NAME_None;
			this->StateClassOverride.DefaultObject = nullptr;
			this->StateClassOverride.Value = nullptr;
		}
		else
		{
			if (!this->StateExtension.IsNone())
			{
				this->RenameNode(this->StateExtension);
			}
			else
			{
				this->RenameNode(this->GetStateGraph()->GetNewStateName());
			}

			if (this->NodeType == EStateNodeType::EXTENDED_NODE)
			{
				this->StateClass = NewObject<UState>(this);
			}
			else
			{
				this->UpdateStateArchetypeOverride();
			}
		}
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateNode, StateExtension))
	{
		if (!this->StateExtension.IsNone())
		{
			this->RenameNode(this->StateExtension);
		}
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