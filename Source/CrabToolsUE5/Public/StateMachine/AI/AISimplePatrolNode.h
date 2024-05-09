#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/AIBaseNode.h"
#include "Actors/PatrolPath.h"
#include "AISimplePatrolNode.generated.h"

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimplePatrolNode : public UAIBaseNode
{
	GENERATED_BODY()
	
	FPatrolPathState PatrolState;

	TWeakObjectPtr<UPathFollowingComponent> FollowComponent;

public:

	virtual void Initialize_Implementation(UStateMachine* POwner) override;
	
};
