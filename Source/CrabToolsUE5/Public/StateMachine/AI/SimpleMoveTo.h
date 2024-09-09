#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/BaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "SimpleMoveTo.generated.h"

USTRUCT(BlueprintType)
struct FMoveToData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	FVector DestinationLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	TObjectPtr<AActor> DestinationActor;

	FMoveToData() : DestinationLocation(0, 0, 0) {}
};

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

public:

	UAISimpleMoveToNode();

	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data) override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	#if WITH_EDITOR
		virtual void PostLinkerChange() override;
		
		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;
	#endif

private:
	void BindCallback();
	void UnbindCallback();

	void MoveTo(UObject* Obj);
};
