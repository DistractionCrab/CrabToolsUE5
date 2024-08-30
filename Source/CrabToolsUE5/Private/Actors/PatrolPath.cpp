#include "Actors/PatrolPath.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "UObject/ObjectSaveContext.h"

APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = true;

	this->SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	this->RootComponent->SetMobility(EComponentMobility::Static);

	// Setup Icon Display in the Editor
	#if WITH_EDITORONLY_DATA
		this->EditorSprite = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));

		ConstructorHelpers::FObjectFinderOptional<UTexture2D> Icon(
			TEXT("/CrabToolsUE5/Icons/PatrolPathIcon.PatrolPathIcon"));

		this->EditorSprite->Sprite = Icon.Get();
		this->EditorSprite->bHiddenInGame = true;
		this->EditorSprite->SetupAttachment(this->RootComponent);
		this->EditorSprite->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
		this->EditorSprite->SetSimulatePhysics(false);
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

void APatrolPath::ToggleDisplay()
{
	if (this->PatrolPoints.Num() != this->Arrows.Num())
	{
		this->InitArrows();
	}

	if (this->IsSelected())
	{
		for (auto& Arrow : this->Arrows) { Arrow->SetVisibility(true); }
	}
	else
	{
		for (auto& Arrow : this->Arrows) { Arrow->SetVisibility(false); }
	}
}

#if WITH_EDITOR
void APatrolPath::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APatrolPath, PatrolPoints))
	{
		this->ClearArrows();
		this->InitArrows();
	}
}

void APatrolPath::ClearArrows()
{
	// If we already have the arrows needed don't bother clearing them.
	if (this->PatrolPoints.Num() == this->Arrows.Num()) { return; }

	for (auto& Arrow : this->Arrows)
	{
		Arrow->DestroyComponent();
	}

	this->Arrows.Empty();
}

void APatrolPath::InitArrows()
{
	for (int Index = 0; Index < this->PatrolPoints.Num(); ++Index) 
	{
		auto Point = this->Get(Index);
		auto NextPoint = this->Get((Index + 1) % this->PatrolPoints.Num());

		FVector Dir = NextPoint - Point;
		UArrowComponent* Arrow = nullptr;

		// If we have the arrows, don't make anymore, just retrieve and update it's information.
		if (this->Arrows.Num() == this->PatrolPoints.Num())
		{
			Arrow = this->Arrows[Index];
		}
		else
		{
			Arrow = NewObject<UArrowComponent>(this);
			Arrow->SetupAttachment(this->RootComponent);
			this->AddInstanceComponent(Arrow);

			Arrow->bHiddenInGame = true;
			Arrow->SetVisibility(true);
			Arrow->bTreatAsASprite = true;
			Arrow->bIsScreenSizeScaled = true;
			Arrow->SetSimulatePhysics(false);
			Arrow->SetRelativeScale3D(FVector(5, 5, 5));
			Arrow->ArrowColor = FColor(4, 99, 32);

			Arrows.Add(Arrow);
		}
		
		Arrow->SetWorldLocation(Point);
		Arrow->SetWorldRotation((NextPoint - Point).Rotation());		
		Arrow->ArrowLength = Dir.Length()/5;
		
		Arrow->RegisterComponent();		
	}

	// Need to re-register this component, otherwise it vanishes when changing (Not permanently, but
	// until the next time the editor is reloaded).
	this->EditorSprite->RegisterComponent();
}

void APatrolPath::PreSave(FObjectPreSaveContext SaveContext)
{
	for (auto& Arrow : this->Arrows)
	{
		Arrow->SetVisibility(false);
	}

	Super::PreSave(SaveContext);
}

void APatrolPath::PostSaveRoot(FObjectPostSaveRootContext SaveContext)
{
	if (this->IsSelected())
	{
		for (auto& Arrow : this->Arrows)
		{
			Arrow->SetVisibility(true);
		}
	}

	Super::PostSaveRoot(SaveContext);
}

#endif

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

