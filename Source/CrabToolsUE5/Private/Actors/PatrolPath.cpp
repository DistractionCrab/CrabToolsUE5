#include "Actors/PatrolPath.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SplineComponent.h"
#include "UObject/ObjectSaveContext.h"

APatrolPath::APatrolPath(): bIsCycle(true)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	auto SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	this->SetRootComponent(SceneComponent);
	this->RootComponent->SetMobility(EComponentMobility::Static);

	// Setup Icon Display in the Editor
	#if WITH_EDITORONLY_DATA
		this->EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));

		ConstructorHelpers::FObjectFinderOptional<UTexture2D> Icon(
			TEXT("/CrabToolsUE5/Icons/PatrolPathIcon.PatrolPathIcon"));

		this->EditorSprite->Sprite = Icon.Get();
		this->EditorSprite->bHiddenInGame = true;
		this->EditorSprite->SetupAttachment(this->RootComponent);
		this->EditorSprite->SetRelativeScale3D_Direct(FVector(0.4f, 0.4f, 0.4f));
		this->EditorSprite->SetRelativeLocation_Direct(50*FVector::UpVector);
		this->EditorSprite->SetSimulatePhysics(false);
		
		this->PathSpline = CreateEditorOnlyDefaultSubobject<USplineComponent>(TEXT("PathDisplay"));

		this->PathSpline->SetClosedLoop(this->bIsCycle);
		this->PathSpline->bHiddenInGame = true;
		this->PathSpline->SetVisibility(false);
		this->PathSpline->bDrawDebug = false;
		this->PathSpline->SetupAttachment(EditorSprite);
		this->PathSpline->SetSimulatePhysics(false);

	#endif

	this->SetCanBeDamaged(false);
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
	// To enable negative indexing.
	i = i % this->PatrolPoints.Num();

	return this->PatrolPoints[i] + this->GetActorLocation();
}

void APatrolPath::ToggleDisplay()
{
	if (this->IsSelected())
	{
		this->InitArrows();
		this->PathSpline->SetVisibility(true);
	} 
	else
	{
		this->PathSpline->SetVisibility(false);
	}
}

#if WITH_EDITOR
void APatrolPath::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APatrolPath, PatrolPoints))
	{
		this->InitArrows();
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APatrolPath, bIsCycle))
	{
		this->PathSpline->SetClosedLoop(this->bIsCycle);
	}
}

void APatrolPath::InitArrows()
{
	this->PathSpline->SetSplineLocalPoints(this->PatrolPoints);

	for (int i = 0; i < this->PatrolPoints.Num(); ++i)
	{
		this->PathSpline->SetTangentAtSplinePoint(i, FVector::Zero(), ESplineCoordinateSpace::World);
	}

	this->EditorSprite->RegisterComponent();
	this->PathSpline->RegisterComponent();
}

void APatrolPath::PreSave(FObjectPreSaveContext SaveContext)
{
	this->PathSpline->SetVisibility(false);
	Super::PreSave(SaveContext);
}

void APatrolPath::PostSaveRoot(FObjectPostSaveRootContext SaveContext)
{
	if (this->IsSelected())
	{
		this->PathSpline->SetVisibility(true);
	}

	Super::PostSaveRoot(SaveContext);
}

#endif

FVector FPatrolPathState::GetTarget(int Offset) const
{
	if (Path)
	{
		auto PointCount = Path->Num();
		
		if (PointCount > 0)
		{
			// Need to reverse the offset if the direction has been reversed.
			Offset = this->bDirection ? Offset : -Offset;

			auto Next = Path->Get(this->CurrentIndex + Offset);

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

void FPatrolPathState::Step()
{
	int Increment = this->bDirection ? 1 : -1;
	if (Path->IsCycle())
	{
		if (this->bDirection)
		{
			if (this->CurrentIndex == Path->Num() - 1)
			{
				this->bDirection = !this->bDirection;
				Increment = -Increment;
			}
		}
		else
		{
			if (this->CurrentIndex == 0)
			{
				this->bDirection = !this->bDirection;
				Increment = -Increment;
			}
		}

		this->CurrentIndex = this->CurrentIndex + Increment;
	}
	else
	{
		this->CurrentIndex = (this->CurrentIndex + Increment) % Path->Num();
	}
}

void FPatrolPathState::SetDirection(int Index)
{
	if (IsValid(this->Path))
	{
		Index = Index % this->Path->Num();

		if (Index >= this->CurrentIndex)
		{
			this->bDirection = true;
		}
		else
		{
			this->bDirection = true;
		}
	}
	else
	{
		this->bDirection = true;
	}
}

void FPatrolPathState::Reset()
{
	this->CurrentIndex = PathIndex;
}

FVector UPatrolPathLibrary::GetTarget(FPatrolPathState& State, int Offset)
{
	return State.GetTarget(Offset);
}


FPatrolPathState::operator bool() const
{
	return this->Path && this->Path->Num() >= 2;
}
