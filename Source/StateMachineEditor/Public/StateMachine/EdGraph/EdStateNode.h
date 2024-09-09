#pragma once

#include "CoreMinimal.h"
#include "StateMachine/IStateMachineLike.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStateNode.generated.h"


UCLASS(CollapseCategories, MinimalAPI)
class UEdStateNode : public UEdBaseStateNode, public IStateLike
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

	UPROPERTY(VisibleAnywhere, Category="StateMachineGraph|Events")
	TSet<FName> NodeEmittedEvents;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineGraph")
	TObjectPtr<UState> StateClass;

public:

	UEdStateNode();
	virtual ~UEdStateNode();
	virtual TSubclassOf<UStateNode> GetNodeClass() const;
	void SetNodeTemplate(UStateNode* NewNode) { this->Nodes.Add(NewNode); }
	virtual FName GetStateName() const override;
	/* Returns the name which should appear on graph nodes. */
	virtual FName GetNodeName() const override { return this->StateName; }
	FName GetStateCategory() const { return this->StateCategory; }
	FName SetStateName(FName NewName);
	const TArray<TObjectPtr<UStateNode>>& GetStateList() const { return this->Nodes; }
	UState* GenerateState(FNodeVerificationContext& Context, UObject* Outer);
	void Delete();
	virtual bool HasEvent(FName EName) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	/* Begin IStateLike Interface */
	virtual TArray<FString> GetEventOptions() const override;
	virtual TArray<FString> GetEnterStates() const override;
	virtual TArray<FString> GetExitStates() const override;
	/* End IStateLike Interface */

	


	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostLinkerChange() override;
	#endif

private:

	void UpdateEmittedEvents();
};