// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class APatrolPath : public AActor
{
	GENERATED_BODY()
	
	USceneComponent* Root;

public:	
	// Sets default values for this actor's properties
	APatrolPath();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TArray<AActor*> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	float LostDistance = 1000;

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual AActor* FindClosestPoint(AActor* Patroller);

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual int FindClosestIndex(AActor* Patroller);

	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* Get(int i) {
		return this->PatrolPoints[i];
	}
};
