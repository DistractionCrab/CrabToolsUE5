#pragma once

#include "CoreMinimal.h"
#include "Actors/Targeting/BaseTargetingActor.h"
#include "ArcTraceTargetingActor.generated.h"

UCLASS(Blueprintable)
class AArcTraceTargetingActor : public ABaseTargetingActor
{
	GENERATED_BODY()

	/* Whether or not to draw debug lines for the trace. */
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug",
		meta = (AllowPrivateAccess))
	bool bDrawDebug = false;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|LineTrace",
		meta = (AllowPrivateAccess))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

	UPROPERTY(BlueprintReadOnly, Category="Target|ArcTrace", meta=(AllowPrivateAccess))
	TObjectPtr<class USplineComponent> PathSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting|ArcTrace",
		meta = (AllowPrivateAccess))
	float HeightAdjust = 100;

protected:

	TArray<AActor*> AddedActors;
	TArray<FVector> AddedPoints;

	/* The actor traced for targeting. */
	TWeakObjectPtr<AActor> TracedActor;
	/* The impact point that was traced by this targeting actor. */
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|LineTrace")
	FVector TracedLocation;


	UPROPERTY(BlueprintreadOnly, Category = "Targeting|LineTrace")
	FVector Base;
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|LineTrace")
	FVector Midpoint;
	UPROPERTY(BlueprintreadOnly, Category = "Targeting|LineTrace")
	FVector Target;

public:

	AArcTraceTargetingActor();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting|LineTrace", meta = (HideSelfPin))
	FVector GetEndPoint() const { return this->TracedLocation; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Targeting|LineTrace", meta = (HideSelfPin))
	AActor* GetEndPointActor() const { return this->TracedActor.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HideSelfPin))
	int GetTargetCount() const { return this->AddedActors.Num(); }

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|LineTrace")
	bool IsValidTarget(AActor* CheckedActor);
	virtual bool IsValidTarget_Implementation(AActor* CheckedActor);

	UFUNCTION(BlueprintNativeEvent, Category = "Targeting|LineTrace")
	void IgnoreActors(TArray<AActor*>& IgnoredActors);
	virtual void IgnoreActors_Implementation(TArray<AActor*>& IgnoredActors);

	UFUNCTION(BlueprintNativeEvent, Category = "Target|LineTrace")
	FVector GetTraceBase() const;
	FVector GetTraceBase_Implementation() const { return this->GetActorLocation(); }

	virtual void PushTarget_Implementation() override;
	virtual void PopTarget_Implementation() override;
	virtual void GetTargets_Implementation(TArray<AActor*>& Actors) const { Actors.Append(this->AddedActors); };
	virtual void GetTargetPoints_Implementation(TArray<FVector>& Points) const override { Points.Append(this->AddedPoints); }

private:
	FORCEINLINE void InvalidateTargetData();
};
