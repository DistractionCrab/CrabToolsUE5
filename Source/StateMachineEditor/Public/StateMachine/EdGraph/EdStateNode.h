#pragma once

#include "CoreMinimal.h"
#include "StateMachine/IStateMachineLike.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStateNode.generated.h"


UCLASS(MinimalAPI)
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

	UPROPERTY(EditDefaultsOnly, Category="StateMachineGraph",
		meta=(GetOptions="GetStateClassesOptions"))
	TSet<FName> StateClasses;

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
	void Delete();
	virtual bool HasEvent(FName EName) override;
	const TSet<FName>& GetStateClasses() { return this->StateClasses; }


	/* Begin IStateLike Interface */
	virtual TArray<FString> GetEventOptions() const override;
	virtual TArray<FString> GetEnterStates() const override;
	virtual TArray<FString> GetExitStates() const override;
	/* End IStateLike Interface */

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;	


	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostLinkerChange() override;
	#endif

private:
	void UpdateEmittedEvents();

	UFUNCTION()
	TArray<FString> GetStateClassesOptions() const;
};