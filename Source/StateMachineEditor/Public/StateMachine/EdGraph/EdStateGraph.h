#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/StateMachine.h"
#include "EdStateGraph.generated.h"

class FGraphActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelected, TArray<class UEdStateNode*>&)
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
};