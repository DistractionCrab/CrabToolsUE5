#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/DataStructures.h"
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
	TObjectPtr<class UEdStateGraph> MainGraph;
	
	UPROPERTY()
	TArray<TObjectPtr<class UEdStateGraph>> SubGraphs;

public:

	/* Editor Events when this object is changed. */
	FStateMachineBlueprintEditorEvents Events;

public:

	/* Stores event names mapped to their descriptions from this SMBP. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "StateMachine")
	void GetEventEntries(TMap<FName, FEventSetRow>& Entries);

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
	/* Returns the set of events defined by this SMBP. */
	TSet<FName> GetEventSet() const;

	// IStateMachineLike Interface
	virtual TArray<FString> GetMachineOptions() const override;
	virtual TArray<FString> GetPropertiesOptions(FSMPropertySearch& SearchParam) const override;

	UStateMachineBlueprintGeneratedClass* GetStateMachineGeneratedClass() const;

private:

	void InspectObject(UObject* Obj);
	FName GetNewGraphName();
};