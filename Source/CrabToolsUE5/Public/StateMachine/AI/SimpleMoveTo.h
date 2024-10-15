#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "SimpleMoveTo.generated.h"

struct FMoveToData;

/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API UAISimpleMoveToNode : public UAIBaseNode
{
	GENERATED_BODY()

private:

	/* The name of the property to get FMovetoData from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|AI",
		meta = (AllowPrivateAccess = true, GetOptions = "GetPropertyOptions"))
	FName PropertyName;
	FSMPropertyReference PropertyRef;

	EPathFollowingResult::Type MovementResult;

	/* Overriden location for extended nodes to use. This is used on a per Enter basis.*/
	FVector OverrideLocation;
	bool bUseOverrideLocation = false;

public:

	UAISimpleMoveToNode();

	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	#if WITH_EDITOR
		virtual void PostLinkerChange() override;
		
		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;
	#endif

protected:

	FMoveToData* GetMovementData() const;

	FORCEINLINE EPathFollowingResult::Type GetMovementResult() const { return this->MovementResult; }

	void StopMovement();
	void SetOverrideLocation(FVector Location);

private:

	void BindCallback();
	void UnbindCallback();

	void MoveTo(UObject* Obj);
};
