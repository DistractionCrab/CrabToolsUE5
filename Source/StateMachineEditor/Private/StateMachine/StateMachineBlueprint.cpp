#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/StateMachineSchema.h"


#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"


UStateMachineBlueprint::UStateMachineBlueprint(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
	EdGraph(nullptr)
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

UEdStateGraph* UStateMachineBlueprint::StateMachineGraph()
{
	if (this->EdGraph == nullptr) {
		this->EdGraph = CastChecked<UEdStateGraph>(FBlueprintEditorUtils::CreateNewGraph(
			this,
			NAME_None,
			UEdStateGraph::StaticClass(),
			UStateMachineSchema::StaticClass()));

		const UEdGraphSchema* Schema = this->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*this->EdGraph);
	}

	return this->EdGraph;
}