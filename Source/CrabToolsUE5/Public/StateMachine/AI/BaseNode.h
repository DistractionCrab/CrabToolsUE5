#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "BaseNode.generated.h"

class AAIController;

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAIBaseNode : public UStateNode
{
	GENERATED_BODY()

	TWeakObjectPtr<APawn> PawnRef;
	TWeakObjectPtr<AAIController> AICtrl;

public:

	virtual void Initialize_Inner_Implementation() override;

	/* Return the AIController that was retrieved by the Pawn onwer of the state machine. */
	FORCEINLINE AAIController* GetAIController() const { return this->AICtrl.Get(); }
	FORCEINLINE APawn* GetPawn() const { return this->PawnRef.Get(); }


	/* Returns whether or not the owner of the state machine is a pawn. */
	FORCEINLINE bool HasPawnOwner() const { return this->PawnRef != nullptr; }
};
