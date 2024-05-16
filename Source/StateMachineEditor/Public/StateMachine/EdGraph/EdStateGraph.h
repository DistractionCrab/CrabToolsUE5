#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "StateMachine/StateMachine.h"
#include "EdStateGraph.generated.h"



UCLASS(MinimalAPI)
class UEdStateGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FNodeSelected, TArray<class UEdStateNode*>)

public:

	UEdStateGraph() {}
	virtual ~UEdStateGraph() {}

	FName GetNewStateName();
};