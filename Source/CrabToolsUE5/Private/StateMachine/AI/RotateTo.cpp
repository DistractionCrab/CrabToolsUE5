#include "StateMachine/AI/RotateTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/AI/Events.h"

UAIRotateToNode::UAIRotateToNode()
{
	this->bRequiresTick = true;
	this->AddEmittedEvent(AI_Events::AI_ARRIVE);
	this->AddEmittedEvent(AI_Events::AI_LOST);
}

void UAIRotateToNode::Tick_Inner_Implementation(float DeltaTime)
{
	if (this->TargetActor.IsValid())
	{
		if (this->FacingCheck())
		{
			this->EmitEvent(AI_Events::AI_ARRIVE);
		}
	}
	else
	{
		this->EmitEvent(AI_Events::AI_LOST);
	}
}

void UAIRotateToNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	FString Address = this->PropertyName.ToString();
	FSMPropertySearch Params = FSMPropertySearch::ObjectProperty(AActor::StaticClass());
	this->PropertyRef = Params.GetProperty<FObjectProperty>(this->GetMachine(), Address);
}

void UAIRotateToNode::Exit_Inner_Implementation()
{
	this->TargetActor = nullptr;
}

void UAIRotateToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Actor = Cast<AActor>(Data))
	{
		this->TargetActor = Actor;

		if (this->TargetActor.IsValid())
		{
			if (auto Ctrl = this->GetAIController())
			{
				Ctrl->SetFocus(this->TargetActor.Get());
			}
			else
			{
				this->TargetActor = nullptr;
			}
		}
	}
}

void UAIRotateToNode::Enter_Inner_Implementation()
{
	if (auto Value = this->PropertyRef.GetValue<AActor>())
	{	
		this->TargetActor = Value;

		if (auto Ctrl = this->GetAIController())
		{
			Ctrl->SetFocus(this->TargetActor.Get());
		}
		else
		{
			this->TargetActor = nullptr;
		}
	}
}

void  UAIRotateToNode::PostTransition_Inner_Implementation()
{
	if (!this->TargetActor.IsValid())
	{
		this->EmitEvent(AI_Events::AI_LOST);
	}
}

bool UAIRotateToNode::FacingCheck() const
{
	// Whether or not we are currently facing the focus point.
	bool Check = false;

	if (auto Ctrl = this->GetAIController())
	{
		if (this->TargetActor.IsValid())
		{
			FVector Facing = this->TargetActor.Get()->GetActorLocation() - this->GetOwner()->GetActorLocation();
			FVector ToTarget = Ctrl->GetFocalPoint() - this->GetOwner()->GetActorLocation();

			if (this->bIgnorePitch)
			{
				// Project the vectors onto the ground.
				Facing.Z = 0;
				ToTarget.Z = 0;				
			}

			Facing.Normalize();
			ToTarget.Normalize();

			Check = FMath::Abs(Facing.Dot(ToTarget) - 1) < 1e-3f;
		}
	}
	else
	{
		Check = true;
	}

	return Check;
}

#if WITH_EDITOR
TArray<FString> UAIRotateToNode::GetPropertyOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		FSMPropertySearch Params = FSMPropertySearch::ObjectProperty(AActor::StaticClass());

		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}

void UAIRotateToNode::PostLinkerChange()
{
	Super::PostLinkerChange();
}
#endif // WITH_EDITOR