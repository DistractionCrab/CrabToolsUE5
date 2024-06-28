#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/AIBaseNode.h"
#include "SimpleMoveTo.generated.h"

inline const char ARRIVE_EVENT[] = "AI_ONARRIVE";
inline const char LOST_EVENT[] = "AI_ONLOST";

/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API USimpleMoveTo : public UAIBaseNode
{
	GENERATED_BODY()
	
	TWeakObjectPtr<class UPathFollowingComponent> FollowComponent;

	UPROPERTY(EditAnywhere, Category="AI/MoveTo", 
		meta = (AllowPrivateAccess = "true"))
	FName OnLostEvent;

	UPROPERTY(EditAnywhere, Category="AI/MoveTo", 
		meta = (AllowPrivateAccess = "true"))
	FName OnArriveEvent;

public:

	USimpleMoveTo();

	virtual void EnterWithData_Implementation(UObject* Data) override;
	virtual void Initialize_Implementation() override;
	
};
