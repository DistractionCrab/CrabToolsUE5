#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "CallDelegateNode.generated.h"


/**
 * Sets a boolean value in a state machine.
 */
UCLASS(Blueprintable, Category = "StateMachine|Properties")
class CRABTOOLSUE5_API UCallDelegateNode : public UStateNode
{
	GENERATED_BODY()

private:

	/* The name of the property to get OnEnterDelegate from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Properties",
		meta = (AllowPrivateAccess = true, GetOptions = "GetPropertyOptions"))
	FName OnEnterDelegate;
	FSMPropertyReference OnEnterRef;

	/* The name of the property to get OnExitDelegate from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Properties",
		meta = (AllowPrivateAccess = true, GetOptions = "GetPropertyOptions"))
	FName OnExitDelegate;
	FSMPropertyReference OnExitRef;

public:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	
	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;
	#endif

private:
	UFUNCTION()
	void DelegateSignatureFunction() {}
};
