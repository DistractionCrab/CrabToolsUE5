#include "Actors/DoorActor.h"

TWeakObjectPtr<UCurveFloat> ADoorActor::SharedCurve = nullptr;

// Sets default values
ADoorActor::ADoorActor()
: PlayRate(1),
	AngleDelta(90),
	CurrentAlpha(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	this->MovementTimeline = CreateDefaultSubobject<UTimelineComponent>(
		TEXT("DoorRotationTimeline"));

	this->RootComponent->SetMobility(EComponentMobility::Movable);
}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	this->Axis = this->GetActorRotation().RotateVector(FVector::UpVector);
	this->BaseRotation = this->GetActorRotation().Quaternion();

	if (!this->MovementCurve)
	{
		this->MovementCurve = this->GetDefaultCurve();
	}	

	this->MovementTimeline->SetPlayRate(this->PlayRate);
	this->MovementTimeline->SetLooping(false);
	this->MovementTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

	this->MovementTrack.BindDynamic(this, &ADoorActor::UpdatePosition);
	this->EndMovementTrack.BindDynamic(this, &ADoorActor::FinishMovement);

	if (this->MovementCurve != nullptr) {
		this->MovementTimeline->AddInterpFloat(this->MovementCurve, MovementTrack);
		this->MovementTimeline->SetTimelineFinishedFunc(EndMovementTrack);
	}
}

UCurveFloat* ADoorActor::GetDefaultCurve()
{
	if (!ADoorActor::SharedCurve.IsValid())
	{
		ADoorActor::SharedCurve = NewObject<UCurveFloat>();

		SharedCurve->FloatCurve.UpdateOrAddKey(0.f, 0.f);
		SharedCurve->FloatCurve.UpdateOrAddKey(1.f, 1.f);
	}

	return ADoorActor::SharedCurve.Get();
}

void ADoorActor::ActivateDoor(bool OpenQ)
{
	this->MovementTimeline->Stop();
	this->MovementTimeline->SetNewTime(this->CurrentAlpha);

	if (OpenQ)
	{
		this->MovementTimeline->Play();
	}
	else
	{
		this->MovementTimeline->Reverse();
	}
}

void ADoorActor::UpdatePosition(float alpha) 
{
	this->CurrentAlpha = alpha;

	float Angle = FMath::Lerp(0.0f, this->AngleDelta, alpha);
	FQuat Rotation(this->Axis, FMath::DegreesToRadians(Angle));

	this->SetActorRotation(Rotation * this->BaseRotation);
}

void ADoorActor::FinishMovement()
{
	this->OnFinishMovementEvent.Broadcast();
}

void ADoorActor::SetPlayRate(float NewPlayRate)
{
	this->PlayRate = NewPlayRate;
	this->MovementTimeline->SetPlayRate(this->PlayRate);
}