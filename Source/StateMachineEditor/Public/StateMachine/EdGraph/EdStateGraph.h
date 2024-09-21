#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/IStateMachineLike.h"
#include "GraphEditAction.h"

#include "EdStateGraph.generated.h"

class UEdEventObject;
class UEdBaseNode;
class UEdStartStateNode;
class UEdStateNode;
class UStateMachineBlueprint;
class UDataTable;
struct FStateMachineArchetypeData;

UENUM(BlueprintType)
enum class EStateMachineGraphType : uint8
{
	EXTENDED_GRAPH    UMETA(DisplayName = "Extends"),
	SUB_GRAPH         UMETA(DisplayName = "Subgraph"),
	MAIN_GRAPH        UMETA(DisplayName = "MainGraph", Hidden),
};

USTRUCT(BlueprintType)
struct FStateMachineArchetypeOverrideContainer
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Instanced, Category = "Override", meta=(ShowInnerProperties, NoResetToDefault))
	TObjectPtr<UStateMachine> Value;

	/* This object is used to compare overriden variables when Linking changes. */
	UPROPERTY()
	TObjectPtr<UStateMachine> DefaultObject;
};

UCLASS(CollapseCategories, MinimalAPI)
class UEdStateGraph : public UEdGraph, public IStateMachineLike
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess,
			EditCondition = "GraphType == EStateMachineGraphType::SUB_GRAPH || GraphType == EStateMachineGraphType::SUB_GRAPH",
			EditConditionHides))
	FName Category;

	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta=(AllowPrivateAccess,
			EditCondition="GraphType != EStateMachineGraphType::MAIN_GRAPH",
			EditConditionHides))
	EStateMachineGraphType GraphType = EStateMachineGraphType::MAIN_GRAPH;

	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (AllowPrivateAccess,
			EditCondition = "GraphType != EStateMachineGraphType::MAIN_GRAPH",
			EditConditionHides))
	bool bInstanceEditable = false;

	/* Whether or not a Blueprint Class variable should be made for this submachine. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, 
			EditCondition="Accessibility == EStateMachineAccessibility::PUBLIC && GraphType == EStateMachineGraphType::SUB_GRAPH",
			EditConditionHides))
	bool bIsVariable = false;

	/* The state machine this graph is utilizing to store its states other data. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess, 
			EditCondition = "GraphType == EStateMachineGraphType::SUB_GRAPH",
			EditConditionHides))
	TObjectPtr<UStateMachine> MachineArchetype;

	/* For extended graphs, the name of the submachine in the parent to override internal data. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess,
			GetOptions = "GetOverrideableMachines",
			EditCondition = "GraphType == EStateMachineGraphType::EXTENDED_GRAPH",
			EditConditionHides))
	FName OverridenMachine;

	/* Reference to a copy of the parent machine archetype, allows for changing variables. */
	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (AllowPrivateAccess,
			EditCondition = "GraphType == EStateMachineGraphType::EXTENDED_GRAPH",
			EditConditionHides))
	FStateMachineArchetypeOverrideContainer MachineArchetypeOverride;

	/* Event sets to be added to this submachine. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine", meta=(RowType = "FEventSetRow"))
	TSet<TObjectPtr<UDataTable>> EventSets;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, 
			EditCondition = "GraphType == EStateMachineGraphType::SUB_GRAPH", 
			EditConditionHides))
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	UPROPERTY()
	TArray<TObjectPtr<UEdEventObject>> EventObjects;

public:

	/* Events that are used by the Graph Editor to communicate. */
	class FEditorEvents
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
	}
	Events;

public:

	UEdStateGraph();
	virtual ~UEdStateGraph() {}

	FName GetNewStateName();	
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
	bool IsEventNameAvailable(FName Name) const;
	bool IsStateNameAvailable(FName Name) const;

	/* Removes the event from this graph. */
	void RemoveEvent(UEdEventObject* EventObj);

	/* Gets a map from their name to their data table values. */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void GetEventEntries(TMap<FName, FEventSetRow>& Entries);

	const TArray<TObjectPtr<UEdEventObject>>& GetEventList() const 
	{ 
		return this->EventObjects; 
	}

	bool CanOverrideStart() const;
	TArray<class UEdStateNode*> GetStates() const;
	TArray<class UEdTransition*> GetTransitions() const;
	TArray<class UEdTransition*> GetExitTransitions(UEdStateNode* Start) const;
	FStateMachineArchetypeData CompileStateMachine(FNodeVerificationContext& Context);
	FName GetStartStateName() const;
	TArray<UEdBaseNode*> GetDestinations(UEdBaseNode* Node) const;
	UEdStartStateNode* GetStartNode() const;
	bool HasEvent(FName EName) const;
	EStateMachineAccessibility GetAccessibility() const { return this->Accessibility; }
	void Inspect();
	bool IsMainGraph() const;
	UStateMachineBlueprint* GetBlueprintOwner() const;
	UStateMachine* GetMachineArchetype() const { return this->MachineArchetype; }
	void Delete();
	void RenameGraph(FName NewName);
	bool IsVariable() const;
	FName GetCategoryName() const;
	FName GetClassPrefix() const;
	FName RenameNode(UEdStateNode* Node, FName NewName);

	FName GetGraphName() const;
	FORCEINLINE void SetGraphType(EStateMachineGraphType GType) { this->GraphType = GType; }
	FORCEINLINE EStateMachineGraphType GetGraphType() const { return this->GraphType; }

	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	bool CanRename() const { return this->GraphType != EStateMachineGraphType::MAIN_GRAPH; }

	//void CollectExtendibleStates(TSet<FString>& StateNames) const;

	#if WITH_EDITOR
		virtual void PostLoad() override;
		virtual void PostEditUndo() override;
		virtual void PostEditChangeProperty(
			FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostLinkerChange() override;

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
	
	TArray<FString> GetInheritableStates(EStateNodeType NodeType) const;

private:

	bool UpdateOverrideData();

	/* Checks whether or not an emitter attached to this graph has an event. */
	bool DoesEmitterHaveEvent(FName EName) const;
	void AppendEmitterEvents(TArray<FString>& Names) const;

	/* Returns the "default" state machine for this graph. */
	UStateMachine* GetSourceMachine() const;

	void VerifyMachineArchetypes(FNodeVerificationContext& Context) const;
	FName GetParentDefinedStartState() const;
	FName GetLocallyDefinedStartState() const;
};