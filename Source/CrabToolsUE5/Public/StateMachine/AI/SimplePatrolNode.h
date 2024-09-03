#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/BaseNode.h"
#include "Actors/PatrolPath.h"
#include "Navigation/PathFollowingComponent.h"
#include "SimplePatrolNode.generated.h"

class UPathFollowingComponent;

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimplePatrolNode : public UAIBaseNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|AI")
	FPatrolPathState PatrolState;

	/* 
	 * This value is used to guard against too many close patrol points. This is needed
	 * since "OnMoveCompleted" will be called if the next patrol point is right next to
	 * the current one, often leading to a StackOverflow.
	*/
	int RecurseGuard = 0;

	UPROPERTY(EditDefaultsOnly, Category="StateMachine|AI",
		meta=(GetOptions="GetPatrolOptions"))
	FName PatrolPathProperty;
	FObjectProperty* PatrolProperty;

	/* Previous states which would not reset the patrolling state. */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|AI",
		meta = (GetOptions = "GetResetStateOptions"))
	TSet<FName> NonResetStates;

public:
	UAISimplePatrolNode();

	virtual void Enter_Implementation() override;
	virtual void Exit_Implementation() override;
	virtual void Initialize_Implementation() override;
	
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	#if WITH_EDITOR
		virtual void PostLinkerChange() override;

		UFUNCTION()
		TArray<FString> GetPatrolOptions() const;

		UFUNCTION()
		TArray<FString> GetResetStateOptions() const;
	#endif

private:

	void MoveToNext();
	void BindCallback();
	void UnbindCallback();
};
