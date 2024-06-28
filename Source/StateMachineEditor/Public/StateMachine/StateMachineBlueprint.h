#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateMachine/EdGraph/EdStateGraph.h"

#include "StateMachineBlueprint.generated.h"

class FStateMachineBlueprintEditorEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FGraphSelected, UEdStateGraph*)
	FGraphSelected OnGraphSelected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FObjectInspected, UObject* Obj)
	FObjectInspected OnObjectInspected;
};

UCLASS(BlueprintType)
class STATEMACHINEEDITOR_API UStateMachineBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY()
	class UEdStateGraph* MainGraph;
	
	UPROPERTY()
	TArray<class UEdStateGraph*> SubGraphs;

public:

	FStateMachineBlueprintEditorEvents Events;

public:
	void SelectGraph(UEdStateGraph* Graph);
	UClass* GetBlueprintClass() const override;
	virtual bool SupportsInputEvents() const override;

	UEdStateGraph* GetMainGraph();
	UEdStateGraph* AddSubGraph();

	bool IsMainGraph(UEdStateGraph* Graph);
	void RenameGraph(UEdStateGraph* Graph, FName Name);
	const TArray<class UEdStateGraph*>& GetSubgraphs() { return this->SubGraphs; }

private:

	void InspectObject(UObject* Obj);
	FName GetNewGraphName();

	/* Returns the list of submachines to use for Hierarchy nodes. */
	UFUNCTION()
	TArray<FString> GetMachineOptions() const;

	/* Used by Hierarchy nodes to generate states for their slot. */
	UFUNCTION()
	TArray<FString> GetSubMachineStateOptions(FName MachineName) const;
};