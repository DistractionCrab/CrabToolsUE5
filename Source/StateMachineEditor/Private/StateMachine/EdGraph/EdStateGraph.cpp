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
#include "Utils/PropertyManagement.h"
#include "StateMachine/DataStructures.h"

#define LOCTEXT_NAMESPACE "EdStateGraph"

constexpr char DEFAULT_NODE_NAME[] = "NewState";
constexpr char DEFAULT_EVENT_NAME[] = "NewEvent";

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

	for (auto& Node : this->Nodes)
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
	if (this->IsMainGraph())
	{
		return this->GetBlueprintOwner()->GeneratedClass->GetFName();
	}
	else
	{
		FString Total;

		Total.Append(this->GetBlueprintOwner()->GeneratedClass->GetName());
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
		if (this->DoesEmitterHaveEvent(EName))
		{
			return true;
		}		
	}	

	return false;
}

bool UEdStateGraph::DoesEmitterHaveEvent(FName EName) const
{
	auto SourceMachine = this->GetSourceMachine();

	for (auto& Emitter : SourceMachine->GetEmitters())
	{
		if (Emitter->GetEmittedEvents().Contains(EName))
		{
			return true;
		}
	}

	return false;
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

	for (auto& Node : this->Nodes)
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

	for (auto& Event : this->EventObjects)
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
	for (auto& EventObj : this->EventObjects)
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

TArray<UEdStateNode*> UEdStateGraph::GetStates() const
{
	TArray<UEdStateNode*> StateNodes;
	this->GetNodesOfClass<UEdStateNode>(StateNodes);

	return StateNodes;
}

TArray<UEdTransition*> UEdStateGraph::GetTransitions() const
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	return Transitions;
}

TArray<UEdTransition*> UEdStateGraph::GetExitTransitions(UEdStateNode* Start) const
{
	TArray<UEdTransition*> Transitions;
	this->GetNodesOfClass<UEdTransition>(Transitions);

	auto Pred = [&](const UEdTransition* A) -> bool { return A->GetStartNode() == Start; };

	return Transitions.FilterByPredicate(Pred);
}

UStateMachineArchetype* UEdStateGraph::GenerateStateMachine(FNodeVerificationContext& Context)
{
	if (this->UpdateOverrideData())
	{
		FString ErrorMessage = FString::Printf(
			TEXT("Subgraph %s:%s overriden statemachine is out of sync with its parent and has been modified."),
			*this->GetBlueprintOwner()->GetName(),
			*this->GetName());
		Context.Warning(ErrorMessage, this);
	}

	this->VerifyMarchineArchetypes(Context);
	UObject* Outer = Context.GetOuter();

	UStateMachineArchetype* StateMachine = NewObject<UStateMachineArchetype>(Outer);

	if (!this->IsMainGraph())
	{
		if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			if (this->OverridenMachine.IsNone() || this->MachineArchetypeOverride.Value == nullptr)
			{
				FString ErrorMessage = FString::Printf(
					TEXT("Subgraph %s specified as extended, but no parent submachine specified to be extended."),
					*this->GetName());
				Context.Error(ErrorMessage, this);

				return nullptr;
			}
			StateMachine->ArchetypeObject = DuplicateObject<UStateMachine>(this->MachineArchetypeOverride.Value.Get(), StateMachine);
		}
		else
		{
			StateMachine->ArchetypeObject = DuplicateObject<UStateMachine>(this->MachineArchetype, StateMachine);
		}
		
		StateMachine->bInstanceEditable = this->bInstanceEditable;
	}

	StateMachine->bIsVariable = this->bIsVariable;
	StateMachine->Accessibility = this->Accessibility;

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

			for (auto& Values : TData)
			{
				StateMachine->AddTransition(State->GetStateName(), Values.Key, Values.Value);
			}
		}
	}

	StateMachine->StartState = this->GetStartStateName();

	return StateMachine;
}

bool UEdStateGraph::IsMainGraph() const
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

	for (auto& Node : this->Nodes)
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

	for (auto& Ev : this->EventObjects)
	{
		Names.Add(Ev->GetEventName().ToString());
	}

	this->AppendEmitterEvents(Names);

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UEdStateGraph::AppendEmitterEvents(TArray<FString>& Names) const
{
	auto SourceMachine = this->GetSourceMachine();

	for (auto& Emitter : SourceMachine->GetEmitters())
	{		
		for (auto& EName : Emitter->GetEmittedEvents())
		{
			Names.Add(EName.ToString());
		}
	}
}

UStateMachine* UEdStateGraph::GetSourceMachine() const
{
	if (this->IsMainGraph())
	{
		return Cast<UStateMachine>(this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetDefaultObject());
	}
	else
	{
		if (this->GraphType == EStateMachineGraphType::SUB_GRAPH)
		{
			return this->MachineArchetype;
		}
		else if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			return this->MachineArchetypeOverride.Value;
		}
	}

	return nullptr;
}

TArray<FString> UEdStateGraph::GetConditionOptions() const
{
	TArray<FString> Names;

	UClass* ClassBase = nullptr;

	if (this->IsMainGraph())
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
	if (!this->IsMainGraph())
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

	if (this->IsMainGraph())
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
	if (!this->IsMainGraph())
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

	if (!this->IsMainGraph())
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

	if (!this->IsMainGraph())
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
	for (auto& EventObj : this->EventObjects)
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

void UEdStateGraph::CollectExtendibleStates(TSet<FString>& StateNames) const
{
	if (this->IsMainGraph())
	{
		this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->CollectExtendibleStates(StateNames);
	}
	else
	{
		this->MachineArchetype->CollectExtendibleStates(StateNames);
	}
}

bool UEdStateGraph::UpdateOverrideData()
{
	bool bOverrideWasModified = false;

	if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
	{
		if (this->OverridenMachine.IsNone())
		{
			this->Accessibility = EStateMachineAccessibility::PRIVATE;
			this->MachineArchetypeOverride.Value = nullptr;
		}
		else if (!this->IsMainGraph()) // Sanity Check.
		{
			if (this->GetFName() != this->OverridenMachine)
			{
				this->RenameGraph(this->OverridenMachine);
			}

			if (!IsValid(this->MachineArchetypeOverride.DefaultObject))
			{
				if (auto BPObj = this->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						this->Accessibility = Class->GetSubMachineAccessibility(this->OverridenMachine);
						this->MachineArchetypeOverride.DefaultObject = Class->DuplicateSubMachineArchetype(this->OverridenMachine, this);
						this->MachineArchetypeOverride.Value = Class->DuplicateSubMachineArchetype(this->OverridenMachine, this);
					}
				}
			}
			else
			{
				if (auto BPObj = this->GetBlueprintOwner())
				{
					if (auto Class = BPObj->GetStateMachineGeneratedClass()->GetParent())
					{
						bOverrideWasModified = UPropertyManagementLibrary::UpdateObjectInheritanceProperties<UStateMachine>(
							Class->GetSubMachineArchetype(this->OverridenMachine),
							this->MachineArchetypeOverride.DefaultObject,
							this->MachineArchetypeOverride.Value);
					}
				}
			}
		}
	}
	else
	{
		this->OverridenMachine = NAME_None;
		this->MachineArchetypeOverride.Value = nullptr;
		this->MachineArchetypeOverride.DefaultObject = nullptr;
	}

	return bOverrideWasModified;
}

void UEdStateGraph::VerifyMarchineArchetypes(FNodeVerificationContext& Context) const
{
	if (!this->IsMainGraph())
	{
		if (this->GraphType == EStateMachineGraphType::EXTENDED_GRAPH)
		{
			if (auto Overriden = this->GetBlueprintOwner()->GetStateMachineGeneratedClass()->GetParent()->GetMostRecentParentArchetype(this->GetFName()))
			{
				auto SourceMachine = this->GetSourceMachine();
				if (!SourceMachine->IsA(Overriden->GetClass()))
				{
					FString ErrorMessage = FString::Printf(
						TEXT("Overriding Archetype is not a subclass of the parent graph's archetype. Parent Class: %s, Overriding Type: %s"),
						*Overriden->GetClass()->GetName(),
						*SourceMachine->GetClass()->GetName());
					Context.Error(ErrorMessage, this);
				}
			}
			else
			{
				FString ErrorMessage = FString::Printf(
					TEXT("Overriding null parent machine archetype in SubGraph %s (Internal Error to be reported)."), 
					*this->GetName());
				Context.Error(ErrorMessage, this);
			}
		}
	}
}

#if WITH_EDITOR

void UEdStateGraph::PostLoad()
{
	Super::PostLoad();
}

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

	this->Modify();

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, EventSets))
	{
		TSet<TObjectPtr<UDataTable>> FilteredEventSets;

		for (auto& DT : this->EventSets)
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
		this->UpdateOverrideData();
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEdStateGraph, MachineArchetypeOverride))
	{
		// Only do this if the value of this was reset to null.
		if (this->MachineArchetypeOverride.Value == nullptr)
		{
			this->UpdateOverrideData();
		}
	}
}

TArray<FString> UEdStateGraph::GetOverrideableMachines() const
{
	TArray<FString> Names;

	if (!this->IsMainGraph())
	{
		if (auto BPObj = this->GetBlueprintOwner())
		{
			if (auto Class = Cast<UStateMachineBlueprintGeneratedClass>(BPObj->GetStateMachineGeneratedClass()->GetSuperClass()))
			{
				Names.Append(Class->GetChildAccessibleSubMachines());

				// Remove any that have been extended.
				for (auto& SubMachine : BPObj->GetDefinedSubMachines())
				{
					Names.Remove(SubMachine);
				}
			}
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

void UEdStateGraph::PostLinkerChange()
{
	
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE