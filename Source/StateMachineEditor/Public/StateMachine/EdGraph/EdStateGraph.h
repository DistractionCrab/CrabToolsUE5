#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/StateMachine.h"
#include "EdStateGraph.generated.h"

class FGraphActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelected, TArray<class UEdGraphNode*>&)
	FNodeSelected OnNodeSelected;
};

UCLASS(MinimalAPI)
class UEdStateGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	/* Events that are used by the Graph Editor to communicate. */
	FGraphActionEvents Events;

public:

	UEdStateGraph() {}
	virtual ~UEdStateGraph() {}

	FName GetNewStateName();
	bool IsNameAvilable(FName Name) const;

	void ClearEvents();
};