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

	virtual void Initialize_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;	
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
	virtual bool RequiresTick_Implementation() const override;
	virtual void PostTransition_Inner_Implementation() override;

	FORCEINLINE void AddNode(UStateNode* Node) { this->Nodes.Add(Node); }

	#if WITH_EDITORONLY_DATA
		virtual void GetEmittedEvents(TSet<FName>& Events) const override;
	#endif
};
