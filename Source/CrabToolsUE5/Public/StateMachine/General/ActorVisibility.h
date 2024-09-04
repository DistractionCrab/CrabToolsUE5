#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "ActorVisibility.generated.h"

/**
 * Node that either hides or shows an actor.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UActorVisibilityNode : public UStateNode
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_DELEGATE(FEventHandler);
	DECLARE_DYNAMIC_DELEGATE_OneParam(FEventWithDataHandler, UObject*, Data);

	UPROPERTY(EditAnywhere, Category = "StateMachine", 
		meta = (AllowPrivateAccess, GetKeyOptions="GetPropertyOptions"))
	FName ActorProperty;

	/* Whether to show or hide the actor on entering this node. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess))
	bool bShowActor = false;

	/* Whether or not the revert to the previous visibility on exit. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess))
	bool bRevertOnExit = true;

public:

	virtual void Initialize_Implementation() override;
	virtual void Enter_Implementation() override;
	virtual void Exit_Implementation() override;	

private:

	UFUNCTION()
	TArray<FString> GetPropertyOptions() const;
};
