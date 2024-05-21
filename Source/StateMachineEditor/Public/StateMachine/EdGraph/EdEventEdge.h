#pragma once

#include "EdEventEdge.generated.h"

class UEdBaseNode;

UCLASS(MinimalAPI)
class UEdEventEdge : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdEventEdge();

	UPROPERTY()
	class UEdGraph* Graph;

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void CreateConnections(UEdBaseNode* Start, UEdBaseNode* End);

	UEdBaseNode* GetStartNode();
	UEdBaseNode* GetEndNode();
};