#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStateNode.generated.h"




UCLASS(MinimalAPI)
class UEdStateNode : public UEdBaseStateNode
{
	GENERATED_BODY()

	/* Whether or not a blueprint variable should be generated for this state. */
	UPROPERTY(EditDefaultsOnly, Category="StateMachineGraph")
	bool bIsVariable;

	UPROPERTY(VisibleAnywhere, Category = "StateMachineGraph")
	FName StateName;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineGraph")
	FName StateCategory;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineGraph")
	TArray<TObjectPtr<UStateNode>> Nodes;


public:
	UEdStateNode();
	virtual ~UEdStateNode();

	virtual TSubclassOf<UStateNode> GetNodeClass() const;

	void SetNodeTemplate(UStateNode* NewNode) { this->Nodes.Add(NewNode); }

	virtual FName GetStateName() const override { return this->StateName; }
	FName GetStateCategory() const { return this->StateCategory; }
	FName SetStateName(FName NewName);

	const TArray<TObjectPtr<UStateNode>>& GetStateList() const { return this->Nodes; }

	UStateNode* GetCompiledNode();
};