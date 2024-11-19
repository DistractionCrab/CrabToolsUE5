#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "SetBooleanNode.generated.h"


/**
 * Sets a boolean value in a state machine.
 */
UCLASS(Blueprintable, Category = "StateMachine|Properties")
class CRABTOOLSUE5_API USetBooleanNode : public UStateNode
{
	GENERATED_BODY()

private:

	/* The name of the property to get FMovetoData from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Properties",
		meta = (AllowPrivateAccess = true, GetOptions = "GetPropertyOptions"))
	FName PropertyName;
	FSMPropertyReference PropertyRef;

	UPROPERTY(EditAnywhere, Category = "StateMachine|Properties", meta = (AllowPrivateAccess))
	bool bOnEnter = true;

	UPROPERTY(EditAnywhere, Category = "StateMachine|Properties", meta = (AllowPrivateAccess))
	bool bOnExit = false;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	
	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;
	#endif
};
