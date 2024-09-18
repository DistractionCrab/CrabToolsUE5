#pragma once

#include "CoreMinimal.h"
#include "StateMachineInterface.generated.h"

/**
 * Simple storage class that contains what public interface a statemachine has.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachineInterface : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TSet<FName> ImplementedEvents;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="StateMachine",
		meta=(AllowPrivateAccess))
	TSet<FName> ImplementedStates;

public:

	const TSet<FName>& GetEvents() const { return this->ImplementedEvents; }
	const TSet<FName>& GetStates() const { return this->ImplementedStates; }

};
