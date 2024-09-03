#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

USTRUCT(BlueprintType)
struct FPatrolPathState
{
	GENERATED_USTRUCT_BODY()

private:

	/* The index in the patrol path to get. */
	UPROPERTY(EditDefaultsOnly, Category="PatrolPath")
	int PathIndex;

public:

	FPatrolPathState() : PathIndex(0) {}

	FVector GetCurrentTarget(APatrolPath* Path);
	FVector GetNextTarget(APatrolPath* Path);
	void Skip();

	void Reset() { this->PathIndex = 0; }
};


UCLASS()
class UPatrolPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="PatrolPath")
	static FVector GetNextTarget(UPARAM(Ref) FPatrolPathState& State, APatrolPath* Path);
};

UCLASS()
class CRABTOOLSUE5_API APatrolPath : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (AllowPrivateAccess = true))
	float LostDistance = 100000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (MakeEditWidget, AllowPrivateAccess=true))
	TArray<FVector> PatrolPoints;

	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		TObjectPtr<class UBillboardComponent> EditorSprite;

		UPROPERTY()
		TObjectPtr<class USplineComponent> PathSpline;
	#endif

public:	

	APatrolPath();	

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual FVector FindClosestPoint(AActor* Patroller);

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual int FindClosestIndex(AActor* Patroller);

	/* Return the patrol point in World Space coordinates. */
	UFUNCTION(BlueprintPure, Category = "AI")
	FVector Get(int i);

	UFUNCTION(BlueprintPure, Category = "AI")
	FORCEINLINE int Num() const { return this->PatrolPoints.Num(); }

	#if WITH_EDITOR
		virtual void PreSave(FObjectPreSaveContext SaveContext) override;
		virtual void PostSaveRoot(FObjectPostSaveRootContext SaveContext) override;
		virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
		void ToggleDisplay();
	#endif

private:

	#if WITH_EDITOR
		void InitArrows();
	#endif
};
