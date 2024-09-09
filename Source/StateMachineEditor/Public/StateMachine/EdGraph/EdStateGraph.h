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

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor",
		meta = (AllowPrivateAccess, EditCondition="Accessibility == EStateMachineAccessibility::PUBLIC",
			EditConditionHides))
	bool bIsVariable = false;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachineEditor",
		meta = (AllowPrivateAccess, 
			EditCondition = "!bIsMainGraph",
			EditConditionHides))
	TObjectPtr<UStateMachine> MachineArchetype;

	UPROPERTY()
	TArray<TObjectPtr<UEdEventObject>> EventObjects;

	UPROPERTY(EditDefaultsOnly, Category = "Events", meta=(RowType = "FEventSetRow"))
	TSet<TObjectPtr<UDataTable>> EventSets;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor",
		meta = (AllowPrivateAccess, 
			EditCondition = "!bIsMainGraph", 
			EditConditionHides))
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	/* Events which can be emitted to parent machines. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true"))
	TSet<FName> EmittedEvents;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true"))
	FName Category;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true",
			GetOptions="GetOverrideableMachines",
			EditCondition="!bIsMainGraph",
			EditConditionHides))
	FName OverridenMachine;

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
	void ClearDelegates();
	void Select();

	/* Creates a new event and returns it. */
	UEdEventObject* CreateEvent();	

	/* 
	 * Attempts to rename the given event to a new name. Returns the name of the event; If the
	 * renaming was successful it'll be To. If the renaming failed, it'll return current name.
	 */
	FName RenameEvent(UEdEventObject* EventObj, FName To);

	/* Returns whether or not the event name is available for a new event. */
	bool IsEventNameAvilable(FName Name) const;

	/* Removes the event from this graph. */
	void RemoveEvent(UEdEventObject* EventObj);

	/* Gets a map from their name to their data table values. */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void GetEventEntries(TMap<FName, FEventSetRow>& Entries);

	const TArray<TObjectPtr<UEdEventObject>>& GetEventList() const 
	{ 
		return this->EventObjects; 
	}

	TArray<class UEdStateNode*> GetStates();
	TArray<class UEdTransition*> GetTransitions();
	TArray<class UEdTransition*> GetExitTransitions(UEdStateNode* Start);
	UStateMachineArchetype* GenerateStateMachine(FNodeVerificationContext& Context);
	FName GetStartStateName() const;
	TArray<UEdBaseNode*> GetDestinations(UEdBaseNode* Node) const;
	UEdStartStateNode* GetStartNode() const;
	bool HasEvent(FName EName) const;
	EStateMachineAccessibility GetAccessibility() const { return this->Accessibility; }
	void Inspect();
	bool IsMainGraph();
	UStateMachineBlueprint* GetBlueprintOwner() const;
	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	UStateMachine* GetMachineArchetype() const { return this->MachineArchetype; }
	void Delete();
	void RenameGraph(FName NewName);
	bool IsVariable() const;
	FName GetCategoryName() const;
	const UStateMachine* GetMarchineArchetype() const { return this->MachineArchetype; }
	FName GetClassPrefix() const;

	#if WITH_EDITOR	
		virtual void PostEditUndo() override;
		virtual void PostEditChangeProperty(
			FPropertyChangedEvent& PropertyChangedEvent) override;
		UFUNCTION()
		TArray<FString> GetOverrideableMachines() const;
	#endif	

	// IStateMachineLike Interface
	virtual TArray<FString> GetMachineOptions() const override;
	virtual TArray<FString> GetStateOptions(const UObject* Asker) const override;
	virtual TArray<FString> GetEventOptions() const override;
	virtual TArray<FString> GetConditionOptions() const override;
	virtual TArray<FString> GetDataConditionOptions() const override;
	virtual TArray<FString> GetPropertiesOptions(FSMPropertySearch& SearchParam) const override;
	//virtual FProperty* GetStateMachineProperty(FString& Address) const override { return nullptr; }
};