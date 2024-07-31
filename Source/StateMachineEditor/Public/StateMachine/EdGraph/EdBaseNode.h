#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "EdBaseNode.generated.h"

class FEdNodeEvents
{
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FNameChanged, FName, FName)
	FNameChanged OnNameChanged;

	DECLARE_MULTICAST_DELEGATE(FNodeDeleted)
	FNodeDeleted OnNodeDeleted;
};

/* Base Graph Node for both states and edges. */
UCLASS(MinimalAPI)
class UEdBaseNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	/* Events for changes to this node. */
	FEdNodeEvents Events;

	virtual void ClearEvents();
	UEdStateGraph* GetStateGraph() const { return Cast<UEdStateGraph>(this->GetGraph()); }

	void Inspect();
};

/* Base State node to be used for states in the State Machine Graph. */
UCLASS(MinimalAPI)
class UEdBaseStateNode : public UEdBaseNode
{
	GENERATED_BODY()

public:
	UEdBaseStateNode() {}
	virtual ~UEdBaseStateNode() {}


	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void AllocateDefaultPins() override;
	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }
	virtual FName GetStateName() const { return NAME_None; }
};