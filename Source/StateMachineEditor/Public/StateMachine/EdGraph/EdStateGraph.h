#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/StateMachine.h"
#include "EdStateGraph.generated.h"

class UEdEventObject;

class FGraphActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelected, TArray<class UEdGraphNode*>&)
	FNodeSelected OnNodeSelected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FObjectInspected, UObject* Obj)
	FObjectInspected OnObjectInspected;

	DECLARE_MULTICAST_DELEGATE_OneParam(FEventCreated, UEdEventObject*)
	FEventCreated OnEventCreated;
};

UCLASS(MinimalAPI)
class UEdStateGraph : public UEdGraph
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, Category = "StateMachineEditor")
	TArray<TObjectPtr<UEdEventObject>> EventObjects;

public:

	/* Events that are used by the Graph Editor to communicate. */
	FGraphActionEvents Events;

public:

	UEdStateGraph() {}
	virtual ~UEdStateGraph() {}

	FName GetNewStateName();
	bool IsStateNameAvilable(FName Name) const;
	bool IsEventNameAvilable(FName Name) const;
	FName RenameEvent(UEdEventObject* EventObj, FName To);
	void ClearDelegates();

	UEdEventObject* CreateEvent();

	const TArray<TObjectPtr<UEdEventObject>>& GetEventList() const 
	{ 
		return this->EventObjects; 
	}

	TArray<TObjectPtr<class UEdStateNode>> GetStates();

	UStateMachine* GenerateStateMachine(UObject* Outer);
};