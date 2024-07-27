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

	virtual TArray<FString> GetEventOptions() const override;

	void Delete();

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override { UE_LOG(LogTemp, Warning, TEXT("EdState::PostCDOCompiled Called")); Super::PostCDOCompiled(Context); }
		virtual void PostLinkerChange() override { UE_LOG(LogTemp, Warning, TEXT("EdState::PostLinkerChange Called")); Super::PostLinkerChange(); }
		virtual void PostReinitProperties() override { UE_LOG(LogTemp, Warning, TEXT("EdState::PostReinitProperties Called")); Super::PostReinitProperties(); }
	#endif
};