#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/IStateMachineLike.h"
#include "GraphEditAction.h"

#include "EdStateGraph.generated.h"

class UEdEventObject;
class UEdBaseNode;
class UEdStartStateNode;
class UEdStateNode;
class UStateMachineArchetype;
class UStateMachineBlueprint;
class UDataTable;

class FGraphActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelected, TArray<class UEdGraphNode*>&)
	FNodeSelected OnNodeSelected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FEventCreated, UEdEventObject*)
	FEventCreated OnEventCreated;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FNameChanged, FName, FName)
	FNameChanged OnNameChanged;

	DECLARE_MULTICAST_DELEGATE(FGraphDeleted)
	FGraphDeleted OnGraphDeleted;

	DECLARE_MULTICAST_DELEGATE_OneParam(FStateAdded, UEdStateNode*)
	FStateAdded OnStateAdded;

	/* Called when undo and redos happen. */
	DECLARE_MULTICAST_DELEGATE(FGraphDataReverted)
	FGraphDataReverted OnGraphDataReverted;
};

UCLASS(MinimalAPI)
class UEdStateGraph : public UEdGraph, public IStateMachineLike
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="StateMachineEditor",
		meta=(AllowPrivateAccess, EditCondition="!bIsMainGraph", EditConditionHides))
	TSubclassOf<UStateMachine> SourceClass;

	UPROPERTY(VisibleAnywhere, Category="StateMachineEditor",
		meta=(ShowInnerProperties, ShowOnlyInnerProperties))
	TArray<TObjectPtr<UEdEventObject>> EventObjects;

	UPROPERTY(EditDefaultsOnly, Category = "Events")
	TSet<TObjectPtr<UDataTable>> EventSets;

public:

	/* Events that are used by the Graph Editor to communicate. */
	FGraphActionEvents Events;

	UPROPERTY(VisibleAnywhere, Category="StateMachineEditor")
	bool bIsMainGraph = false;

public:

	UEdStateGraph();
	virtual ~UEdStateGraph() {}

	FName GetNewStateName();
	bool IsStateNameAvilable(FName Name) const;
	bool IsEventNameAvilable(FName Name) const;
	FName RenameEvent(UEdEventObject* EventObj, FName To);
	void RemoveEvent(UEdEventObject* EventObj);
	void ClearDelegates();
	void Select();
	UClass* GetSourceClass() { return this->SourceClass.Get(); }

	UEdEventObject* CreateEvent();

	const TArray<TObjectPtr<UEdEventObject>>& GetEventList() const 
	{ 
		return this->EventObjects; 
	}

	TArray<class UEdStateNode*> GetStates();
	TArray<class UEdTransition*> GetTransitions();
	TArray<class UEdTransition*> GetExitTransitions(UEdStateNode* Start);
	UStateMachineArchetype* GenerateStateMachine(FKismetCompilerContext& Context);
	FName GetStartStateName();
	TArray<UEdBaseNode*> GetDestinations(UEdBaseNode* Node) const;
	UEdStartStateNode* GetStartNode() const;
	bool HasEvent(FName EName) const;

	void Inspect();

	bool IsMainGraph();
	UStateMachineBlueprint* GetBlueprintOwner() const;
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	#if WITH_EDITOR	
		virtual void PostEditUndo() override;
		virtual void PostEditChangeProperty(
			FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif

	void Delete();
	void RenameGraph(FName NewName);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void GetEventEntries(TMap<FName, FString>& Entries);

	// IStateMachineLike Interface
	virtual TArray<FString> GetMachineOptions() const override;
	virtual TArray<FString> GetStateOptions() const override;
	virtual TArray<FString> GetEventOptions() const override;
	virtual TArray<FString> GetConditionOptions() const override;
	virtual UClass* GetStateMachineClass() override;
};