// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

USTRUCT(BlueprintType)
struct FPatrolPathState {
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<APatrolPath> PatrolRef;

	UPROPERTY()
	int Index = 0;

	AActor* GetNextTarget(APatrolPath* Path);

	void Reset() { this->Index = 0; }
};


UCLASS()
class UPatrolPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="PatrolPath")
	static AActor* GetNextTarget(UPARAM(Ref) FPatrolPathState& State, APatrolPath* Path);
};

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
