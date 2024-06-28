#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/StateMachine.h"
#include "GraphEditAction.h"
#include "EdStateGraph.generated.h"

class UEdEventObject;
class UEdBaseNode;
class UEdStartStateNode;
class UEdStateNode;

class FGraphActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelected, TArray<class UEdGraphNode*>&)
	FNodeSelected OnNodeSelected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FEventCreated, UEdEventObject*)
	FEventCreated OnEventCreated;

	DECLARE_MULTICAST_DELEGATE_OneParam(FNameChanged, FName)
	FNameChanged OnNameChanged;

	DECLARE_MULTICAST_DELEGATE(FGraphDeleted)
	FGraphDeleted OnGraphDeleted;

	DECLARE_MULTICAST_DELEGATE_OneParam(FStateAdded, UEdStateNode*)
	FStateAdded OnStateAdded;
};

UCLASS(MinimalAPI)
class UEdStateGraph : public UEdGraph
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category="StateMachineEditor",
		meta=(AllowPrivateAccess, EditCondition="!bIsMainGraph", EditConditionHides))
	TSubclassOf<UStateMachine> SourceClass;

	UPROPERTY(VisibleAnywhere, Category="StateMachineEditor",
		meta=(ShowInnerProperties, ShowOnlyInnerProperties))
	TArray<TObjectPtr<UEdEventObject>> EventObjects;

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
	void ClearDelegates();
	void Select();

	UEdEventObject* CreateEvent();

	const TArray<TObjectPtr<UEdEventObject>>& GetEventList() const 
	{ 
		return this->EventObjects; 
	}

	TArray<TObjectPtr<class UEdStateNode>> GetStates();
	UStateMachine* GenerateStateMachine(UObject* Outer);
	FName GetStartStateName();
	TArray<UEdBaseNode*> GetDestinations(UEdBaseNode* Node) const;
	UEdStartStateNode* GetStartNode() const;

	void Inspect();

	bool IsMainGraph();
	class UStateMachineBlueprint* GetBlueprintOwner() const;

	virtual void NotifyGraphChanged(const FEdGraphEditAction& Action) override;

	UFUNCTION()
	TArray<FString> GetStateOptions() const;

	UFUNCTION()
	TArray<FString> GetEventOptions() const;

	UFUNCTION()
	TArray<FString> GetConditionOptions() const;
};