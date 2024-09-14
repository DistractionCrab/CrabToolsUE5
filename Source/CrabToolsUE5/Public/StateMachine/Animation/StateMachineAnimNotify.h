#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "StateMachineAnimNotify.generated.h"

class USkeleton;

/* Anim Notify used to send a specified event to an EventListener interface*/
UCLASS()
class UStateMachineAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	FName EventToEmit;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override {}
};