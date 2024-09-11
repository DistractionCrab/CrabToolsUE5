#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "StateMachine/EdGraph/StateMachineSchema.h"
#include "StateMachine/DataStructures.h"
#include "Engine/DataTable.h"

#define LOCTEXT_NAMESPACE "UStateMachineBlueprint"
#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define DEFAULT_STATEMACHINE_NAME "NewStateMachine"

UStateMachineBlueprint::UStateMachineBlueprint(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
	MainGraph(nullptr)
{

}

UClass* UStateMachineBlueprint::GetBlueprintClass() const
{
	return UStateMachineBlueprintGeneratedClass::StaticClass();
}

bool UStateMachineBlueprint::SupportsInputEvents() const 
{
	return true;
}

UEdStateGraph* UStateMachineBlueprint::GetMainGraph()
{
	if (this->MainGraph == nullptr)
	{
		this->MainGraph = CastChecked<UEdStateGraph>(FBlueprintEditorUtils::CreateNewGraph(
			this,
			"MainGraph",
			UEdStateGraph::StaticClass(),
			UStateMachineSchema::StaticClass()));

		//this->MainGraph->bIsMainGraph = true;
		this->MainGraph->SetGraphType(EStateMachineGraphType::MAIN_GRAPH);
		const UEdGraphSchema* Schema = this->MainGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*this->MainGraph);
	}

	return this->MainGraph;
}

UEdStateGraph* UStateMachineBlueprint::AddSubGraph()
{
	auto NewGraph = CastChecked<UEdStateGraph>(FBlueprintEditorUtils::CreateNewGraph(
		this,
		this->GetNewGraphName(),
		UEdStateGraph::StaticClass(),
		UStateMachineSchema::StaticClass()));

	NewGraph->SetGraphType(EStateMachineGraphType::SUB_GRAPH);

	const UEdGraphSchema* Schema = NewGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*NewGraph);

	this->SubGraphs.Add(NewGraph);

	return NewGraph;
}

FName UStateMachineBlueprint::GetNewGraphName()
{
	FString DefaultName(DEFAULT_STATEMACHINE_NAME);
	int index = 0;

	bool IsAvailable = false;

	while (!IsAvailable)
	{
		index += 1;

		FString NameCheck(DefaultName);
		NameCheck.AppendInt(index);

		IsAvailable = this->IsGraphNameAvailable(NameCheck);		
	}

	DefaultName.AppendInt(index);

	return FName(DefaultName);
}

bool UStateMachineBlueprint::IsGraphNameAvailable(FString& Name) const
{
	if (Name == this->MainGraph->GetName())
	{
		return false;
	}
	else
	{
		for (auto& SubGraph : this->SubGraphs)
		{
			if (SubGraph->GetName() == Name)
			{
				return false;
			}
		}

		if (auto BPGC = this->GetStateMachineGeneratedClass())
		{
			return !BPGC->IsSubMachineNameInUse(Name);
		}
		else
		{
			return true;
		}
	}
}

bool UStateMachineBlueprint::IsMainGraph(const UEdStateGraph* Graph) const
{
	return this->MainGraph == Graph;
}

void UStateMachineBlueprint::RenameGraph(UEdStateGraph* Graph, FName Name)
{
	auto Renamed = this->SubGraphs.Find(Graph);

	if (Renamed >= 0)
	{
		const FScopedTransaction Transaction(LOCTEXT("RenameGraph", "Rename Graph"));
		this->Modify();
		Graph->Modify();

		FName OldName = Graph->GetFName();
		Graph->Rename(*Name.ToString(), this);
		Graph->Events.OnNameChanged.Broadcast(OldName, Name);
	}
}

void UStateMachineBlueprint::SelectGraph(UEdStateGraph* Graph)
{
	if (Graph == this->MainGraph)
	{
		this->Events.OnGraphSelected.Broadcast(Graph);
	}
	else if (this->SubGraphs.Contains(Graph))
	{
		this->Events.OnGraphSelected.Broadcast(Graph);
	}
}

TArray<FString> UStateMachineBlueprint::GetDefinedSubMachines() const
{
	TArray<FString> Names;

	for (auto& SubM : this->SubGraphs)
	{
		Names.Add(SubM->GetName());
	}

	return Names;
}

void UStateMachineBlueprint::InspectObject(UObject* Obj)
{
	this->Events.OnObjectInspected.Broadcast(Obj);
}

TArray<FString> UStateMachineBlueprint::GetMachineOptions() const
{
	TSet<FString> Names;

	for (auto Graph : this->SubGraphs)
	{
		Names.Add(Graph->GetName());	
	}

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GeneratedClass))
	{
		Names.Append(BPGC->GetChildAccessibleSubMachines());
	}
	
	return Names.Array();
}

TSet<FName> UStateMachineBlueprint::GetEventSet() const
{
	TSet<FName> EventNames;

	for (auto& Ev : this->MainGraph->GetEventList())
	{
		EventNames.Add(Ev->GetEventName());
	}

	for (auto& SubGraph : this->SubGraphs)
	{
		for (auto& Ev : SubGraph->GetEventList())
		{
			EventNames.Add(Ev->GetEventName());
		}
	}

	return EventNames;
}

TArray<FString> UStateMachineBlueprint::GetPropertiesOptions(FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names;

	for (TFieldIterator<FProperty> FIT(this->GeneratedClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FProperty* f = *FIT;

		if (SearchParam.Matches(f))
		{
			Names.Add(f->GetName());
		}
	}

	for (auto SubMachine : this->SubGraphs)
	{
		for (TFieldIterator<FProperty> FIT(SubMachine->GetMachineArchetype()->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (SearchParam.Matches(f))
			{
				FString Formatted = FString::Printf(TEXT("%s/%s"), *SubMachine->GetName(), *f->GetName());
				Names.Add(Formatted);
			}
		}
	}

	return Names;
}

void UStateMachineBlueprint::DeleteGraph(UEdStateGraph* Graph)
{
	if (this->SubGraphs.Contains(Graph))
	{
		const FScopedTransaction Transaction(LOCTEXT("DeleteStateMachine", "Delete State Machine"));
		this->Modify();
		this->SubGraphs.Remove(Graph);
		Graph->Events.OnGraphDeleted.Broadcast();
	}
}

void UStateMachineBlueprint::ClearDelegates()
{
	this->Events.OnGraphSelected.Clear();
	this->Events.OnObjectInspected.Clear();
}

void UStateMachineBlueprint::GetEventEntries(TMap<FName, FEventSetRow>& Entries)
{
	this->MainGraph->GetEventEntries(Entries);

	for (auto Graph : this->SubGraphs)
	{
		Graph->GetEventEntries(Entries);
	}
}

void UStateMachineBlueprint::AddEventsToDataTable(UDataTable* EventSet, bool bClearEvents)
{
	if (EventSet->RowStruct == FEventSetRow::StaticStruct())
	{
		const FScopedTransaction Transaction(LOCTEXT("EditEventSet", "Add To Event Set"));
		EventSet->Modify();

		if (bClearEvents)
		{
			EventSet->EmptyTable();
		}

		TMap<FName, FEventSetRow> EntryMap;
		this->GetEventEntries(EntryMap);

		for (auto Entry : EntryMap)
		{
			if (!EventSet->FindRow<FEventSetRow>(Entry.Key, "", false))
			{
				EventSet->AddRow(Entry.Key, Entry.Value);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Data Table has wrong type of row."));
	}
}

UStateMachineBlueprintGeneratedClass* UStateMachineBlueprint::GetStateMachineGeneratedClass() const
{
	return Cast<UStateMachineBlueprintGeneratedClass>(this->GeneratedClass);
}

#undef LOCTEXT_NAMESPACE