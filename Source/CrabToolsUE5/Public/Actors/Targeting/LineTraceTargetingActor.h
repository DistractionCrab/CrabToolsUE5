#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTargetingActor.h"
#include "LineTraceTargetingActor.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class ULineTraceTargeterInterface : public UInterface
{
	GENERATED_BODY()
};

class ILineTraceTargeterInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting|LineTrace")
	FVector GetEndPoint() const;
	virtual FVector GetEndPoint_Implementation() const { return FVector::Zero(); }
};

UCLASS(Blueprintable)
class ALineTraceTargetingActor : public ABaseTargetingActor
{
	GENERATED_BODY()

	/* Whether or not to draw debug lines for the trace. */
	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug",
			meta = (AllowPrivateAccess))
		bool bDrawDebug = false;
	#endif // WITH_EDITORONLY_DATA

	/* Scale the direction to the target by the given percentage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|LineTrace",
		meta = (AllowPrivateAccess, Units="Percent"))
	float CorrectionFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|LineTrace",
		meta = (AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

	/* The actor traced for targeting. */
	TWeakObjectPtr<AActor> TracedActor;
	/* The impact point that was traced by this targeting actor. */
	FVector TracedLocation;

public:

	ALineTraceTargetingActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Targeting|LineTrace", meta=(HideSelfPin))
	FVector GetEndPoint() const { return this->TracedLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting|LineTrace", meta = (HideSelfPin))
	AActor* GetEndPointActor() const { return this->TracedActor.Get(); }
};
