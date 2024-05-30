#pragma once

#include "EdTransition.generated.h"

class UEdBaseNode;

USTRUCT(BlueprintType)
struct FEventTransitionData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor")
	FName Condition = FName("TrueCondition");

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor")
	FName DataCondition = FName("TrueDataCondition");
};

UCLASS(MinimalAPI)
class UEdTransition : public UEdBaseNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor")
	TMap<FName, FEventTransitionData> EventToConditionMap;

public:
	UEdTransition();

	UPROPERTY()
	class UEdGraph* Graph;

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void CreateConnections(UEdBaseStateNode* Start, UEdBaseStateNode* End);

	UEdBaseStateNode* GetStartNode();
	UEdBaseStateNode* GetEndNode();
};