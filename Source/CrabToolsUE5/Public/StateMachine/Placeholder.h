#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "Placeholder.generated.h"

/**
 * State Machine Node used as a mapped placeholder to be replaced. Has no other functionality.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UPlaceholderNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	FName SlotName;

	UPROPERTY()
	TObjectPtr<UStateNode> SubNode;

public:

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	FORCEINLINE FName GetSlotName() { return this->SlotName; }

	virtual void Initialize_Inner_Implementation() override;

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Event_Inner_Implementation(FName Event) override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void ExitWithData_Inner_Implementation(UObject* Data) override;
	virtual void PostTransition_Inner_Implementation() override;
};
