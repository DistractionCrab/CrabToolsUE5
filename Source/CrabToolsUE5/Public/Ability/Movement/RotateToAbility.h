#pragma once

#include "CoreMinimal.h"
#include "Ability/Ability.h"
#include "RotateToAbility.generated.h"

/**
 * Ability which rotates a character around the z-axis (yaw) towards a target or target point.
 */
UCLASS(Blueprintable, meta = (DisplayName = "RotateTo"))
class CRABTOOLSUE5_API URotateToAbility : public UAbility
{
	GENERATED_BODY()

	FVector TargetPoint;

	UPROPERTY(EditDefaultsOnly, Category="Ability|Movement",
		meta = (AllowPrivateAccess,
			ClampMin = "0.0", ClampMax = "10800.0", UIMin = "0", UIMax = "10800.0",
			Units="DegreesPerSecond"))
	float RotateSpeed = 360.0f;

public:

	UFUNCTION(BlueprintCallable, Category="Ability|Movement")
	void SetTargetActor(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Ability|Movement")
	void SetTargetPoint(const FVector& Target);

protected:

	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual bool RequiresTick_Inner_Implementation() const override { return true; }
};
