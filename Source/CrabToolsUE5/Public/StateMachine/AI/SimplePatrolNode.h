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
	
	FPatrolPathState PatrolState;

	UPROPERTY(EditDefaultsOnly, Category="StateMachine|AI",
		meta=(GetOptions="GetPatrolOptions"))
	FName PatrolPathProperty;
	FObjectProperty* PatrolProperty;

public:

	virtual void Enter_Implementation() override;
	virtual void Initialize_Implementation() override;
	
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetPatrolOptions() const;
	#endif

private:
	void MoveToNext();
};
