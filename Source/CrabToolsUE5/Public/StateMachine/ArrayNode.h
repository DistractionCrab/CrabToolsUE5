#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "ArrayNode.generated.h"

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UArrayNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Instanced, Category = "ArrayNode", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UStateNode>> Nodes;

public:

	virtual void Initialize_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Event_Implementation(FName Event) override;
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void EnterWithData_Implementation(UObject* Data) override;
	virtual void Exit_Implementation() override;	
	virtual void ExitWithData_Implementation(UObject* Data) override;

	FORCEINLINE void AddNode(UStateNode* Node) { this->Nodes.Add(Node); }
};
