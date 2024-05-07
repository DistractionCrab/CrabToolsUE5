#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraphSchema_K2.h"
#include "StateMachine/Widgets/Nodes/EdStateNode.h"
#include "StateMachineSchema.generated.h"


USTRUCT()
struct GENERICGRAPHEDITOR_API FSMSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

private:


public:
	FSMSchemaAction_NewNode(): NodeTemplate(nullptr) {}

	FSMSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	UEdStateNode* NodeTemplate;
};


UCLASS(MinimalAPI)
class UStateMachineSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BackwardCompatibilityNodeConversion(
		UEdGraph* Graph, bool bOnlySafeChanges) const override;

	virtual EGraphType GetGraphType(const UEdGraph* TestEdGraph) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

private:
	/*
	void ConvertAnimationEventNodes(UEdGraph* Graph) const;

	void ConvertAddAnimationDelegate(UEdGraph* Graph) const;
	void ConvertRemoveAnimationDelegate(UEdGraph* Graph) const;
	void ConvertClearAnimationDelegate(UEdGraph* Graph) const;

	void ReplaceAnimationFunctionAndAllocateDefaultPins(UEdGraph* Graph, UK2Node* OldNode, UK2Node_CallFunction* NewFunctionNode) const;

	void FixDefaultToSelfForAnimation(UEdGraph* Graph) const;
	*/
};