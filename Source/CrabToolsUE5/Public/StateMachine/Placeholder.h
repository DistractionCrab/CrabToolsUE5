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

	virtual void Initialize_Implementation() override;

	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Event_Implementation(FName Event) override;
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void EnterWithData_Implementation(UObject* Data) override;
	virtual void Exit_Implementation() override;
	virtual void ExitWithData_Implementation(UObject* Data) override;
	virtual void PostTransition_Implementation() override;
};
