// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "ElevatorSystem.generated.h"

UCLASS()
class AElevatorSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevatorSystem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elevator System")
	TArray<AActor*> Targets;

	
	UTimelineComponent* MovementTimeline;
	FOnTimelineFloat MovementTrack;
	FOnTimelineEvent EndMovementTrack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elevator System")
	UCurveFloat* MovementCurve;
	UPROPERTY()
	//TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	TArray<int> MovementSequence;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Elevator System")
	int PositionIndex;
	bool Reverse;


private:
	void StartMovement() {
		if (this->HasTwoPoints()) {
			this->MovementTimeline->PlayFromStart();
		}
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetMovementStart();
	FVector GetMovementDirection();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Elevator System")
	virtual void Step();

	UFUNCTION(BlueprintCallable, Category = "Elevator System")
	virtual void UpdatePosition(float alpha);

	UFUNCTION(BlueprintCallable, Category = "Elevator System")
	void GoToIndex(int i);
	

	FORCEINLINE bool HasTwoTargets() { return this->Targets.Num() >= 2; }
	FORCEINLINE bool HasTwoPoints() { return this->MovementSequence.Num() >= 2; }
	
	UFUNCTION()
	void FinishedMovement();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Elevator System")
	void PostFinishCallback();
};
