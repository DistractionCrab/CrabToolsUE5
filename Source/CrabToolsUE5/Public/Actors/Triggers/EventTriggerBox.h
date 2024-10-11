#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachineInterface.h"
#include "Engine/TriggerBox.h"
#include "EventTriggerBox.generated.h"

UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API AEventTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

	/* The interfaces to pull possible events from. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess))
	TArray<TSoftObjectPtr<UStateMachineInterface>> Interfaces;

	/* The event emitted to an overlapped actor. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess, GetOptions = "GetEventOptions"))
	FName BeginEvent;

	/* The event emitted to an overlapped actor that no longer is overlapping. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess, GetOptions = "GetEventOptions"))
	FName EndEvent;

	/* Whether or not to use data events vs basic events. */
	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess))
	bool bUseData;

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (AllowPrivateAccess, EditCondition = "bUseData", EditConditionHides))
	TSoftObjectPtr<UObject> Data;

protected:

	virtual void BeginPlay() override;

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		UFUNCTION()
		TArray<FString> GetEventOptions() const;
	#endif // WITH_EDITOR

protected:

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};