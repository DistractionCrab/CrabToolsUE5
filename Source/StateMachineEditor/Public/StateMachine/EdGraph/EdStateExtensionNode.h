#pragma once

#include "CoreMinimal.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStateExtensionNode.generated.h"

class UState;
class UStateNode;

UCLASS(CollapseCategories, MinimalAPI)
class UEdStateExtensionNode : public UEdBaseStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "StateMachineGraph",
		meta=(GetOptions="GetExtendibleStates"))
	FName StateName;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineGraph")
	TArray<TObjectPtr<UStateNode>> Nodes;

	UPROPERTY(VisibleAnywhere, Category="StateMachineGraph|Events")
	TSet<FName> NodeEmittedEvents;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "StateMachineGraph",
		meta=(ShowInnerProperties))
	TObjectPtr<UState> StateClass;

	UPROPERTY(EditAnywhere, Category = "StateMachineGraph")
	bool bOverrideParent = false;

public:

	UEdStateExtensionNode();
	virtual ~UEdStateExtensionNode();
	virtual TSubclassOf<UStateNode> GetNodeClass() const;
	void SetNodeTemplate(UStateNode* NewNode) { this->Nodes.Add(NewNode); }
	/* Returns the name which is actually stored in compilation */
	virtual FName GetStateName() const override;
	/* Returns the name which should appear on graph nodes. */
	virtual FName GetNodeName() const override { return this->StateName; }
	const TArray<TObjectPtr<UStateNode>>& GetStateList() const { return this->Nodes; }
	UState* GenerateState(FNodeVerificationContext& Context, UObject* Outer);
	void Delete();
	virtual bool HasEvent(FName EName) override;
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	/* Returns true if the inherited node is overrideable, otherwise is protected. */
	bool IsOverride() const;

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

	#if WITH_EDITOR
		TArray<FString> GetExtendibleStates() const;
	#endif // WITH_EDITOR

	void UpdateEmittedEvents();
};