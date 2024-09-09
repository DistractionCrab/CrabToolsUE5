#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "AnimNotifyWait.generated.h"

class USkeleton;

/**
 * Node that waits for an animation notification
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UAnimNotifyWaitNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(AllowPrivateAccess))
	TObjectPtr<USkeleton> Skeleton;

	UPROPERTY(EditDefaultsOnly, Category="Animation", meta=(AllowPrivateAccess, GetOptions="GetNotifyOptions"))
	FName AnimNotify;

public:

	UAnimNotifyWaitNode();

	virtual void Initialize_Inner_Implementation() override;

private:
	UFUNCTION()
	TArray<FString> GetNotifyOptions() const;
};
