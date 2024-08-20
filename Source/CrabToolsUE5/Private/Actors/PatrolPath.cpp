#include "Actors/PatrolPath.h"
#include "Components/BillboardComponent.h"
#include "Interfaces/IPluginManager.h"

// Sets default values
APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

	this->Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(this->Root);
	
	this->EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));

	// Setup Icon Display in the Editor
	#if WITH_EDITORONLY_DATA
		ConstructorHelpers::FObjectFinderOptional<UTexture2D> Icon(
			TEXT("/CrabToolsUE5/Icons/PatrolPathIcon.PatrolPathIcon"));

		this->EditorSprite->Sprite = Icon.Get();
		this->EditorSprite->bHiddenInGame = true;
		this->EditorSprite->SetupAttachment(this->Root);
		this->EditorSprite->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	#endif
}

FVector APatrolPath::FindClosestPoint(AActor* Patroller)
{
	if (Patroller == nullptr) {
		return this->GetActorLocation();
	}

	FVector Goal = this->GetActorLocation();
	float Dist = this->LostDistance * this->LostDistance;

	for (auto& p : this->PatrolPoints)
	{
		float DSquared = FVector::DistSquared(p, Patroller->GetActorLocation());
		if (DSquared < Dist)
		{
			Goal = p;
			Dist = DSquared;
		}
	}
	return Goal;
}

int APatrolPath::FindClosestIndex(AActor* Patroller) {
	if (Patroller == nullptr)
	{
		return -1;
	} 
	else if (this->PatrolPoints.Num() == 0)
	{
		return -1;
	}

	int Goal = 0;
	float Dist = this->LostDistance * this->LostDistance;

	for (int i = 0; i < this->PatrolPoints.Num(); ++i)
	{
		auto p = this->Get(i);
		float DSquared = FVector::DistSquared(p, Patroller->GetActorLocation());

		if (DSquared < Dist) {
			Goal = i;
			Dist = DSquared;
		}
	}

	return Goal;
}

FVector APatrolPath::Get(int i)
{
	return this->PatrolPoints[i] + this->GetActorLocation();
}

FVector FPatrolPathState::GetNextTarget(APatrolPath* Path)
{
	if (Path)
	{
		auto PointCount = Path->Num();
		if (PointCount > 0)
		{
			this->Index += 1;
			auto Next = Path->Get(this->Index);
			this->Index %= PointCount;

			return Next;
		}
		else
		{
			return Path->GetActorLocation();
		}
	}
	else
	{
		return FVector::Zero();
	}
}

FVector FPatrolPathState::GetCurrentTarget(APatrolPath* Path)
{
	if (Path)
	{
		auto PointCount = Path->Num();
		if (PointCount > 0)
		{
			this->Index %= PointCount;
			auto Next = Path->Get(this->Index);

			return Next;
		}
		else
		{
			return Path->GetActorLocation();
		}
	}
	else
	{
		return FVector::Zero();
	}
}

void FPatrolPathState::Skip()
{
	this->Index += 1;
}

FVector UPatrolPathLibrary::GetNextTarget(FPatrolPathState& State, APatrolPath* Path)
{
	return State.GetNextTarget(Path);
}

