#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "OnHitNode.generated.h"

/**
 * Simple node for making an entity follow a PatrolPath actor's path.
 */
UCLASS(Blueprintable, Category = "StateMachine|Physics")
class CRABTOOLSUE5_API UOnHitNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="StateMachine|Physics", meta=(AllowPrivateAccess))
	TSubclassOf<UPrimitiveComponent> Component;

public:

	UOnHitNode();

protected:

	virtual void Enter_Inner_Implementation() override;
	virtual void Exit_Inner_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, Category="StateMachine|Physics")
	void OnHitDetected(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void OnHitDetected_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {}

private:

	UFUNCTION()
	void OnHitDetected_Internal(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
