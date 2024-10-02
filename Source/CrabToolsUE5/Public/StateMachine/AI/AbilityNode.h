#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "AbilityNode.generated.h"

class UAbility;

UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAbilityNode : public UStateNode
{
	GENERATED_BODY()

	/* The default ability to perform when entering this state. */
	UPROPERTY(BlueprintReadOnly, Instanced, Category = "Ability", meta=(AllowPrivateAccess))
	TObjectPtr<UAbility> DefaultAbility;

	/* The ability to start when entering this node. */
	TObjectPtr<UAbility> Selected;

public:

	UAbilityNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;

private:
	UFUNCTION()
	void HandleFinish(UAbility* Abi);
};
