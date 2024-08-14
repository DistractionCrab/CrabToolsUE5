// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

USTRUCT(BlueprintType)
struct FPatrolPathState {
	GENERATED_BODY()

private:

	int Index;

public:

	FPatrolPathState() : Index(0) {}

	AActor* GetCurrentTarget(APatrolPath* Path);
	AActor* GetNextTarget(APatrolPath* Path);
	void Skip();

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

	UPROPERTY()
	TObjectPtr<USceneComponent> Root;

protected:

	UPROPERTY()
	TObjectPtr<class UBillboardComponent> EditorSprite;

public:	
	// Sets default values for this actor's properties
	APatrolPath();	

public:	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI)
	float LostDistance = 100000;

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual AActor* FindClosestPoint(AActor* Patroller);

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual int FindClosestIndex(AActor* Patroller);

	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* Get(int i) {
		return this->PatrolPoints[i];
	}

	UFUNCTION(BlueprintPure, Category = "AI")
	FORCEINLINE int Num() const { return this->PatrolPoints.Num(); }
};
