#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "PlaceholderNode.generated.h"

/**
 * State Machine Node used as a mapped placeholder to be replaced. Has no other functionality.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UPlaceholderNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="ProcStateMachine")
	FName SlotName;

	UPROPERTY()
	TObjectPtr<UStateNode> SubNode;

public:

	UFUNCTION(BlueprintCallable, Category="ProcStateMachine")
	FORCEINLINE FName GetSlotName() { return this->SlotName; }

	virtual void Initialize_Implementation() override;

	virtual void Tick_Implementation(float DeltaTime) override { if (this->SubNode) { this->SubNode->Tick_Internal(DeltaTime); } }
	virtual void Event_Implementation(FName Event) override { if (this->SubNode) { this->SubNode->Event_Internal(Event); } }
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override { if (this->SubNode) { this->SubNode->EventWithData_Internal(EName, Data); } }
	virtual void Enter_Implementation() override { if (this->SubNode) { this->SubNode->Enter_Internal(); } }
	virtual void EnterWithData_Implementation(UObject* Data) override { if (this->SubNode) { this->SubNode->EnterWithData_Internal(Data); } }
	virtual void Exit_Implementation() override { if (this->SubNode) { this->SubNode->Exit_Internal(); } }
	virtual void ExitWithData_Implementation(UObject* Data) override { if (this->SubNode) { this->SubNode->ExitWithData_Internal(Data); } }
};
