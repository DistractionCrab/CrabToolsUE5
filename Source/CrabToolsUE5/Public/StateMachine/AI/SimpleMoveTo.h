#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/AIBaseNode.h"
#include "Navigation/PathFollowingComponent.h"
#include "SimpleMoveTo.generated.h"

USTRUCT(BlueprintType)
struct  FMoveToData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	FVector DestinationLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	TObjectPtr<AActor> DestinationActor;
};

/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI")
class CRABTOOLSUE5_API USimpleMoveTo : public UAIBaseNode
{
	GENERATED_BODY()

private:

	static FName ARRIVE_EVENT;
	static FName LOST_EVENT;
	TWeakObjectPtr<class UPathFollowingComponent> FollowComponent;

private:

	UPROPERTY(EditAnywhere, Category = "StateMachine|AI",
		meta = (AllowPrivateAccess = true, GetOptions = "GetPropertyOptions"))
	FName DestinationData;

	FStructProperty* DataPropRef;

public:

	USimpleMoveTo();

	virtual void EnterWithData_Implementation(UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void Initialize_Implementation() override;
	
	UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	#if WITH_EDITOR
		virtual void PostLinkerChange() override;
	#endif

	#if WITH_EDITORONLY_DATA
		UFUNCTION()
		TArray<FString> GetPropertyOptions();
	#endif
};
