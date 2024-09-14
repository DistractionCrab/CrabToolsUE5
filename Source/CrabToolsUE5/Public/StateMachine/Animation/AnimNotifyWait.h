#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "AnimNotifyWait.generated.h"

/**
 * Node that waits for an animation notification. Uses a constant Notify name of
 * SM_FinishWaitState
 */
UCLASS(Blueprintable, Category = "StateMachine|Animation")
class CRABTOOLSUE5_API UAnimNotifyWaitNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess))
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(AllowPrivateAccess, GetOptions="GetNotifyOptions"))
	FName ComponentName;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> Component;

public:

	UAnimNotifyWaitNode();

	virtual void Initialize_Inner_Implementation() override;
	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

private:

	UFUNCTION()
	TArray<FString> GetNotifyOptions() const;

	UFUNCTION()
	void AnimNotify_SM_FinishWaitState();
};
