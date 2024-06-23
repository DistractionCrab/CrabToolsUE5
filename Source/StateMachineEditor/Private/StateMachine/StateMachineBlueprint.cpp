#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/StateMachineSchema.h"


#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define DEFAULT_STATEMACHINE_NAME "NewStateMachine"

UStateMachineBlueprint::UStateMachineBlueprint(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
	MainGraph(nullptr)
{
	//this->StateMachineGraph();
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
	if (this->MainGraph == nullptr) {
		this->MainGraph = CastChecked<UEdStateGraph>(FBlueprintEditorUtils::CreateNewGraph(
			this,
			"MainGraph",
			UEdStateGraph::StaticClass(),
			UStateMachineSchema::StaticClass()));

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

	const UEdGraphSchema* Schema = NewGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*NewGraph);

	this->SubGraphs.Add(NewGraph);

	return NewGraph;
}

FName UStateMachineBlueprint::GetNewGraphName()
{
	FString DefaultName(DEFAULT_STATEMACHINE_NAME);
	int index = 0;

	for (auto Graph : this->SubGraphs)
	{
		FString GraphName = Graph->GetFName().ToString();

		if (GraphName.StartsWith(DEFAULT_STATEMACHINE_NAME))
		{
			FString EndName = GraphName.RightChop(DefaultName.Len());

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

	return FName(DefaultName);
}

bool UStateMachineBlueprint::IsMainGraph(UEdStateGraph* Graph)
{
	return this->MainGraph == Graph;
}

void UStateMachineBlueprint::RenameGraph(UEdStateGraph* Graph, FName Name)
{
	auto Renamed = this->SubGraphs.Find(Graph);

	if (Renamed)
	{
		Graph->Rename(*Name.ToString(), this);
		Graph->Events.OnNameChanged.Broadcast(Name);
	}
}

void UStateMachineBlueprint::SelectGraph(UEdStateGraph* Graph)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempging to Select Graph"));

	if (Graph == this->MainGraph)
	{
		UE_LOG(LogTemp, Warning, TEXT("Main Graph Case..."));
		this->Events.OnGraphSelected.Broadcast(Graph);
	}
	else if (this->SubGraphs.Contains(Graph))
	{
		UE_LOG(LogTemp, Warning, TEXT("SubGraph Case..."));
		this->Events.OnGraphSelected.Broadcast(Graph);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Else Case...?"));
	}
}

void UStateMachineBlueprint::InspectObject(UObject* Obj)
{
	this->Events.OnObjectInspected.Broadcast(Obj);
}