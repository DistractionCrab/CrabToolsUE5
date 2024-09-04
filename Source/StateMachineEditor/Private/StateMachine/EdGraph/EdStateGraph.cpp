#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "KismetCompiler.h"
#include "Engine/DataTable.h"
#include "StateMachine/DataStructures.h"

#define LOCTEXT_NAMESPACE "EdStateGraph"
#define DEFAULT_NODE_NAME "NewState"
#define DEFAULT_EVENT_NAME "NewEvent"

UEdStateGraph::UEdStateGraph(): Accessibility(EStateMachineAccessibility::PRIVATE)
{
	this->MachineArchetype = this->CreateDefaultSubobject<UStateMachine>(TEXT("Default Machine"));
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
	else if (Action.Action & EEdGraphActionType::GRAPHACTION_RemoveNode)
	{
		for (auto Node : Action.Nodes)
		{
			UEdStateNode* CastNode = Cast<UEdStateNode>(const_cast<UEdGraphNode*>(Node));

			if (CastNode)
			{
				CastNode->Events.OnNodeDeleted.Broadcast();
			}
		}
	}

	this->Modify();
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

FName UEdStateGraph::GetClassPrefix() const
{
	if (this->bIsMainGraph)
	{
		return this->GetBlueprintOwner()->GeneratedClass->GetFName();
	}
	else
	{
		FString Total;

		Total.Append(this->MachineArchetype->GetClass()->GetFName().ToString());
		Total.Append("::");
		Total.Append(this->GetName());

		return FName(Total);
	}
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

FName UEdStateGraph::GetStartStateName() const
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
				return NAME_None;
			}
		}
	}

	return NAME_None;
}

bool UEdStateGraph::HasEvent(FName EName) const
{
	auto Ev = this->EventObjects.FindByPredicate([&](const UEdEventObject* Ev) { return Ev->GetEventName() == EName; });

	if (Ev)
	{
		return true;
	}
	else
	{
		for (auto DT : this->EventSets)
		{
			if (DT->FindRow<FEventSetRow>(EName, ""))
			{
				return true;
			}
		}
		return false;
	}	
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
		FString EventName = Event->GetEventName().ToString();

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

	const FScopedTransaction Transaction(LOCTEXT("CreateEvent", "Create Event"));
	this->Modify();

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
		if (EventObj->GetEventName() == Name)
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
		return EventObj->GetEventName();
	}
}

TArray<UEdStateNode*> UEdStateGraph::GetStates()
{
	TArray<UEdStateNode*> StateNodes;
	this->GetNodesOfClass<UEdStateNode>(StateNodes);

	return StateNodes;
}

TArray<UEdTransition*> UEdStateGraph::GetTransitions()
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	return Transitions;
}

TArray<UEdTransition*> UEdStateGraph::GetExitTransitions(UEdStateNode* Start)
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	auto Pred = [&](const UEdTransition* A) -> bool { return A->GetStartNode() == Start; };

	return Transitions.FilterByPredicate(Pred);
}

UStateMachineArchetype* UEdStateGraph::GenerateStateMachine(FNodeVerificationContext& Context)
{
	UObject* Outer = Context.GetOuter();

	UStateMachineArchetype* StateMachine = NewObject<UStateMachineArchetype>(Outer);

	if (!this->bIsMainGraph)
	{
		StateMachine->ArchetypeObject = DuplicateObject<UStateMachine>(this->MachineArchetype, StateMachine);
	}

	StateMachine->bIsVariable = this->bIsVariable;

	for (auto State : this->GetStates())
	{
		// Compile the full state data for the state.
		{
			UState* BuiltState = State->GenerateState(Context, StateMachine);

			StateMachine->AddStateData(State->GetStateName(), BuiltState);
		}

		for (auto Transition : this->GetExitTransitions(State))
		{
			auto TData = Transition->GetTransitionData(Context);

			for (auto Values : TData)
			{
				StateMachine->AddTransition(State->GetStateName(), Values.Key, Values.Value);
			}
		}
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

bool UEdStateGraph::IsVariable() const 
{
	return StateMachineAccessibility::IsPublic(this->Accessibility) && this->bIsVariable;
}

UStateMachineBlueprint* UEdStateGraph::GetBlueprintOwner() const
{
	return Cast<UStateMachineBlueprint>(this->GetOuter());
}

void UEdStateGraph::Select()
{
	this->GetBlueprintOwner()->SelectGraph(this);
	this->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
}

void UEdStateGraph::Inspect()
{
	this->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
}

TArray<FString> UEdStateGraph::GetStateOptions(const UObject* Asker) const
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
		Names.Add(Ev->GetEventName().ToString());
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetConditionOptions() const
{
	TArray<FString> Names;

	UClass* ClassBase = nullptr;

	if (this->bIsMainGraph)
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
	}
	else
	{
		if (this->MachineArchetype)
		{
			ClassBase = this->MachineArchetype->GetClass();
		}
		else
		{
			return Names;
		}
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

	// If it's a submachine, allow binding to parent functions.
	if (!this->bIsMainGraph)
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
		FnArchetype = ClassBase->FindFunctionByName("TrueCondition");

		for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			UFunction* f = *FIT;

			if (f->IsSignatureCompatibleWith(FnArchetype) && !f->HasMetaData("IsDefaultCondition"))
			{
				Names.Add("../" + f->GetName());
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetDataConditionOptions() const
{
	TArray<FString> Names;

	UClass* ClassBase = nullptr;

	if (this->bIsMainGraph)
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
	}
	else
	{
		if (this->MachineArchetype)
		{
			ClassBase = this->MachineArchetype->GetClass();
		}
		else
		{
			return Names;
		}
	}

	// Find the example function to check against to find others of the same signature.
	auto FnArchetype = ClassBase->FindFunctionByName("TrueDataCondition");

	for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(FnArchetype))
		{
			Names.Add(f->GetName());
		}
	}

	// If it's a submachine, allow binding to parent functions.
	if (!this->bIsMainGraph)
	{
		ClassBase = this->GetBlueprintOwner()->GeneratedClass;
		FnArchetype = ClassBase->FindFunctionByName("TrueDataCondition");

		for (TFieldIterator<UFunction> FIT(ClassBase, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			UFunction* f = *FIT;

			if (f->IsSignatureCompatibleWith(FnArchetype) && !f->HasMetaData("IsDefaultCondition"))
			{
				Names.Add("../" + f->GetName());
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetMachineOptions() const
{
	TArray<FString> Names = this->GetBlueprintOwner()->GetMachineOptions();

	if (!this->bIsMainGraph)
	{
		// Need to make sure the references are to the parent machine.
		TArray<FString> NewNames;

		for (auto& OldName : Names)
		{
			NewNames.Add("../" + OldName);
		}

		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->MachineArchetype->GetClass()))
		{
			NewNames.Append(BPGC->GetChildAccessibleSubMachines());
		}

		Names = NewNames;
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UEdStateGraph::GetPropertiesOptions(FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names = this->GetBlueprintOwner()->GetPropertiesOptions(SearchParam);

	if (!this->bIsMainGraph)
	{
		TArray<FString> NewNames;

		for (auto& Name : Names)
		{
			NewNames.Add("../" + Name);
		}

		for (TFieldIterator<FProperty> FIT(this->MachineArchetype->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (SearchParam.Matches(f))
			{
				NewNames.Add(f->GetName());
			}
		}

		Names = NewNames;
	}

	return Names;
}

void UEdStateGraph::Delete()
{
	this->GetBlueprintOwner()->DeleteGraph(this);
}

void UEdStateGraph::RenameGraph(FName NewName)
{
	this->GetBlueprintOwner()->RenameGraph(this, NewName);
}

void UEdStateGraph::RemoveEvent(UEdEventObject* EventObj)
{
	if (this->EventObjects.Contains(EventObj))
	{
		this->EventObjects.Remove(EventObj);
		EventObj->Events.OnEventRemoved.Broadcast();
	}
}

bool UEdStateGraph::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetBlueprintOwner()->Modify(bAlwaysMarkDirty);	
}

void UEdStateGraph::GetEventEntries(TMap<FName, FEventSetRow>& Entries)
{
	for (auto EventObj : this->EventObjects)
	{
		Entries.Add(EventObj->GetEventName(), EventObj->GetDescription());
	}
}

FName UEdStateGraph::GetCategoryName() const
{
	if (this->Category == NAME_None)
	{
		return "SubMachines";
	}
	else
	{
		FString CategoryName;
		CategoryName.Append("SubMachines/");
		CategoryName.Append(this->Category.ToString());

		return FName(CategoryName);
	}
}

#if WITH_EDITOR

void UEdStateGraph::PostEditUndo()
{
	Super::PostEditUndo();
	UEdGraph::NotifyGraphChanged();
	this->Events.OnGraphDataReverted.Broadcast();
}

void UEdStateGraph::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UE_LOG(LogTemp, Warning, TEXT("Updated EdStateGraphProperty, attempting modify."));

	this->Modify();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, EventSets))
	{
		TSet<TObjectPtr<UDataTable>> FilteredEventSets;

		for (auto DT : this->EventSets)
		{
			if (DT->GetRowStruct() == FEventSetRow::StaticStruct())
			{
				FilteredEventSets.Add(DT);
			}
		}

		this->EventSets = FilteredEventSets;
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, Accessibility))
	{
		if (!StateMachineAccessibility::IsPublic(this->Accessibility))
		{
			this->bIsVariable = false;
		}
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, OverridenMachine))
	{
		if (this->OverridenMachine.IsNone())
		{
			this->Accessibility = EStateMachineAccessibility::PRIVATE;
		}
		else
		{
			if (!this->bIsMainGraph)
			{
				if (auto BPObj = this->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass())
					{
						this->Accessibility = Class->GetSubMachineAccessibility(this->OverridenMachine);
					}
				}
			}
		}
	}
}

TArray<FString> UEdStateGraph::GetOverrideableMachines() const
{
	TArray<FString> Names;

	if (!this->bIsMainGraph)
	{
		if (auto BPObj = this->GetBlueprintOwner())
		{
			if (auto Class = BPObj->GetStateMachineGeneratedClass())
			{
				Names.Append(Class->GetChildAccessibleSubMachines());
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE