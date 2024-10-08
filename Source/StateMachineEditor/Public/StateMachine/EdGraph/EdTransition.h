#pragma once

#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdTransition.generated.h"

class UTransitionCondition;
class UTransitionDataCondition;
class UEdBaseNode;
class UEdBaseStateNode;
class UEdStateGraph;

USTRUCT(BlueprintType)
struct FEventTransitionData
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor",
	//	meta=(GetOptions="GetConditionOptions"))
	//FName Condition = FName("TrueCondition");

	//UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor",
	//	meta = (GetOptions = "GetDataConditionOptions"))
	//FName DataCondition = FName("TrueDataCondition");

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineEditor")
	TObjectPtr<UTransitionCondition> Condition;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineEditor")
	TObjectPtr<UTransitionDataCondition> DataCondition;
};

UCLASS(MinimalAPI)
class UEdTransition : public UEdBaseNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="StateMachineEditor",
		meta=(GetKeyOptions="GetEventOptions"))
	TMap<FName, FEventTransitionData> EventToConditionMap;

public:
	UEdTransition();

	UPROPERTY()
	class UEdGraph* Graph;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void CreateConnections(UEdBaseStateNode* Start, UEdBaseStateNode* End);
	void Delete();

	UEdBaseStateNode* GetStartNode() const;
	UEdBaseStateNode* GetEndNode() const;
	TMap<FName, FTransitionData> GetTransitionData(FNodeVerificationContext& Context);

	UFUNCTION()
	TArray<FString> GetEventOptions() const;
};