#pragma once

#include "CoreMinimal.h"
#include "StateMachine/AI/BaseNode.h"
#include "RotateToNode.generated.h"


/**
 * Simple node for making an entity move to a given actor.
 */
UCLASS(Blueprintable, Category = "StateMachine|AI", meta=(DisplayName="RotateTo"))
class CRABTOOLSUE5_API UAIRotateToNode : public UAIBaseNode
{
	GENERATED_BODY()

private:

	/* The name of the property to get FMovetoData from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|AI",
		meta = (AllowPrivateAccess = true, GetOptions = "GetPropertyOptions"))
	FName PropertyName;
	FSMPropertyReference PropertyRef;
	TWeakObjectPtr<AActor> TargetActor;


	UPROPERTY(EditAnywhere, Category = "StateMachine|AI",
		meta = (AllowPrivateAccess = true))
	bool bIgnorePitch = true;

public:

	UAIRotateToNode();

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void EnterWithData_Inner_Implementation(UObject* Data) override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;
	virtual void Initialize_Inner_Implementation() override;
	virtual void PostTransition_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override { return true; }
	
	#if WITH_EDITOR
		virtual void PostLinkerChange() override;
		
		UFUNCTION()
		TArray<FString> GetPropertyOptions() const;
	#endif

private:
	bool FacingCheck() const;
};
