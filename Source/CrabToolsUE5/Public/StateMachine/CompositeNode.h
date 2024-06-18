#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "CompositeNode.generated.h"

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UCompositeNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Instanced, Category = "CompositeNode", meta = (AllowPrivateAccess = "true"))
	TMap<FName, UStateNode*> Nodes;

public:

	virtual void Initialize_Implementation(UStateMachine* POwner) override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Event_Implementation(FName Event) override;
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void EnterWithData_Implementation(UObject* Data) override;
	virtual void Exit_Implementation() override;	
	virtual void ExitWithData_Implementation(UObject* Data) override;
	virtual UStateNode* Substitute(FName SlotName, UStateNode* Node) override;
	virtual UStateNode* ExtractAs(TSubclassOf<UStateNode> Class) override;
};
