#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"
#include "StateMachine/Schema/StateMachineSchema.h"


#include "EdGraph/EdGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"


UStateMachineBlueprint::UStateMachineBlueprint(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer),
	EdGraph(nullptr)
{
}

UClass* UStateMachineBlueprint::GetBlueprintClass() const {
	return UStateMachineBlueprintGeneratedClass::StaticClass();
}

bool UStateMachineBlueprint::SupportsInputEvents() const  {
	return true;
}

UEdGraph* UStateMachineBlueprint::SMGraph() {
	if (this->EdGraph == nullptr) {
		this->EdGraph = CastChecked<UEdGraph>(FBlueprintEditorUtils::CreateNewGraph(
			this,
			NAME_None,
			UEdGraph::StaticClass(),
			UStateMachineSchema::StaticClass()));


		this->EdGraph->AddNode(NewObject<UEdStartStateNode>(this->EdGraph), false, false);
		//this->EdGraph->CreateNode(UEdStateNode::StaticClass(), false);
	}

	return this->EdGraph;
}