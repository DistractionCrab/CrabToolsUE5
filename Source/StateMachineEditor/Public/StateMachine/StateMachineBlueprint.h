#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachineBlueprint.generated.h"

class UDataTable;

class FStateMachineBlueprintEditorEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FGraphSelected, UEdStateGraph*)
	FGraphSelected OnGraphSelected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FObjectInspected, UObject* Obj)
	FObjectInspected OnObjectInspected;
};

UCLASS(BlueprintType)
class STATEMACHINEEDITOR_API UStateMachineBlueprint
: public UBlueprint, 
	public IStateMachineLike
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

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StateMachine")
	void GetEventEntries(TMap<FName, FString>& Entries);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StateMachine")
	void AddEventsToDataTable(UDataTable* EventSet, bool bClearEvents=false);

	void SelectGraph(UEdStateGraph* Graph);
	UClass* GetBlueprintClass() const override;
	virtual bool SupportsInputEvents() const override;

	UEdStateGraph* GetMainGraph();
	UEdStateGraph* AddSubGraph();

	bool IsMainGraph(UEdStateGraph* Graph);
	void RenameGraph(UEdStateGraph* Graph, FName Name);
	void DeleteGraph(UEdStateGraph* Graph);
	void ClearDelegates();
	const TArray<class UEdStateGraph*>& GetSubgraphs() { return this->SubGraphs; }

	// IStateMachineLike Interface
	virtual TArray<FString> GetMachineOptions() const override;
	virtual UClass* GetStateMachineClass() override { return this->GeneratedClass; }

private:

	void InspectObject(UObject* Obj);
	FName GetNewGraphName();

	/* Used by Hierarchy nodes to generate states for their slot. */
	UFUNCTION()
	TArray<FString> GetSubMachineStateOptions(FName MachineName) const;	
};