#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

class APatrolPath;

USTRUCT(BlueprintType)
struct FPatrolPathState
{
	GENERATED_USTRUCT_BODY()

private:

	/* The initial index in the patrol path to get. */
	UPROPERTY(EditAnywhere, Category="PatrolPath")
	int PathIndex;
	int CurrentIndex;

	/* Whether or not we are following the path forward.*/
	UPROPERTY(EditAnywhere, Category = "PatrolPath")
	bool bDirection;

	UPROPERTY(EditAnywhere, Category = "PatrolPath")
	TObjectPtr<APatrolPath> Path;

public:

	FPatrolPathState() : PathIndex(0), CurrentIndex(0), bDirection(true) {}

	FVector GetTarget(int Offset=0) const;
	void Step();
	void Reset();
	int GetIndex() const { return this->CurrentIndex; }
	APatrolPath* GetPath() const { return this->Path; }
	void SetDirection(int Index);
	void SetDirection(bool NewDirection) { this->bDirection = NewDirection; }

	/* Return whether or not this state can be valid to traverse. Requires a path and at least two points. */
	operator bool() const;
};


UCLASS()
class UPatrolPathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="PatrolPath")
	static FVector GetTarget(UPARAM(Ref) FPatrolPathState& State, int Offset=0);
};

UCLASS()
class CRABTOOLSUE5_API APatrolPath : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (AllowPrivateAccess))
	bool bIsCycle = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (AllowPrivateAccess))
	float LostDistance = 100000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI,
		meta = (MakeEditWidget, AllowPrivateAccess))
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
	FORCEINLINE bool IsCycle() const { return this->bIsCycle; }
	void SetIsCycle(bool bNewCycleState) { this->bIsCycle = bNewCycleState; }

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
