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

AActor* APatrolPath::FindClosestPoint(AActor* Patroller) {
	if (Patroller == nullptr) {
		return this;
	}

	AActor* Goal = Patroller;
	float Dist = this->LostDistance * this->LostDistance;

	for (AActor* p : this->PatrolPoints) {
		float DSquared = FVector::DistSquared(p->GetActorLocation(), Patroller->GetActorLocation());
		if (DSquared < Dist) {
			Goal = p;
			Dist = DSquared;
		}
	}
	return Goal;
}

int APatrolPath::FindClosestIndex(AActor* Patroller) {
	if (Patroller == nullptr) {
		return -1;
	} else if (this->PatrolPoints.Num() == 0) {
		return -1;
	}

	int Goal = 0;
	float Dist = this->LostDistance * this->LostDistance;

	for (int i = 0; i < this->PatrolPoints.Num(); ++i) {
		AActor* p = this->PatrolPoints[i];
		float DSquared = FVector::DistSquared(p->GetActorLocation(), Patroller->GetActorLocation());
		if (DSquared < Dist) {
			Goal = i;
			Dist = DSquared;
		}
	}

	return Goal;
}

AActor* FPatrolPathState::GetNextTarget(APatrolPath* Path)
{
	if (Path)
	{
		auto PointCount = Path->PatrolPoints.Num();
		if (PointCount > 0)
		{
			this->Index += 1;
			auto Next = Path->PatrolPoints[this->Index];
			this->Index %= PointCount;

			return Next;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

AActor* FPatrolPathState::GetCurrentTarget(APatrolPath* Path)
{
	if (Path)
	{
		auto PointCount = Path->PatrolPoints.Num();
		if (PointCount > 0)
		{
			this->Index %= PointCount;
			auto Next = Path->PatrolPoints[this->Index];

			return Next;
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

void FPatrolPathState::Skip()
{
	this->Index += 1;
}

AActor* UPatrolPathLibrary::GetNextTarget(FPatrolPathState& State, APatrolPath* Path) {
	return State.GetNextTarget(Path);
}