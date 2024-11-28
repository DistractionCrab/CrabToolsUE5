#include "StateMachine/AI/SimplePatrolNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/Events.h"
#include "StateMachine/Logging.h"

#define LOCTEXT_NAMESPACE "AISimplePatrolNode"

UAISimplePatrolNode::UAISimplePatrolNode()
{
	this->AddEmittedEvent(Events::AI::ARRIVE);
	this->AddEmittedEvent(Events::AI::LOST);
}

void UAISimplePatrolNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	FString Address = this->PropertyName.ToString();
	FSMPropertySearch Params = FSMPropertySearch::StructProperty(FPatrolPathState::StaticStruct());
	this->PropertyRef = Params.GetProperty<FStructProperty>(this->GetMachine(), Address);

	check(this->PropertyRef);
	check(this->GetAIController());
}

void UAISimplePatrolNode::PostTransition_Inner_Implementation()
{
	bool bDoReset = true;

	for (auto& IncState : this->NonResetStates)
	{
		if (IncState.StateName == this->GetMachine()->GetPreviousStateName())
		{
			bDoReset = false;
		}
	}

	if (bDoReset)
	{
		this->GetState()->Reset();
	}

	this->BindCallback();	
	this->MoveToNext();
}

void UAISimplePatrolNode::Exit_Inner_Implementation()
{
	this->UnbindCallback();

	this->GetAIController()->StopMovement();
}

void UAISimplePatrolNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	this->GetState()->Step();

	switch (Result)
	{
		case EPathFollowingResult::Success:
			this->EmitEvent(Events::AI::ARRIVE);
			if (this->Active())
			{
				this->MoveToNext();
			}
			break;
		case EPathFollowingResult::Blocked:
			this->MoveToNext();
			break;
		default:
			this->EmitEvent(Events::AI::LOST);
			break;
	}

}

void UAISimplePatrolNode::MoveToNext()
{
	auto State = this->GetState();

	if (!IsValid(State->GetPath())) { return; }

	auto Ctrl = this->GetAIController();
	FVector Goal = State->GetTarget();

	this->RecurseGuard += 1;

	if (this->RecurseGuard > State->GetPath()->Num())
	{
		// If we've recursed too many times, then remove the call back.
		this->GetAIController()->ReceiveMoveCompleted.RemoveAll(this);
	}

	Ctrl->MoveToLocation(Goal);

	this->RecurseGuard = 0;
}

FPatrolPathState* UAISimplePatrolNode::GetState() const
{
	auto State = this->PropertyRef.GetValue<FPatrolPathState>();

	check(State);

	return State;
}

void UAISimplePatrolNode::BindCallback()
{
	this->GetAIController()->ReceiveMoveCompleted.AddDynamic(this, &UAISimplePatrolNode::OnMoveCompleted);
}

void UAISimplePatrolNode::UnbindCallback()
{
	if (auto CtrlQ = this->GetAIController())
	{
		CtrlQ->ReceiveMoveCompleted.RemoveAll(this);
	}
}

#if WITH_EDITOR
TArray<FString> UAISimplePatrolNode::GetPatrolOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		FSMPropertySearch Params = FSMPropertySearch::StructProperty(FPatrolPathState::StaticStruct());

		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}

TArray<FString> UAISimplePatrolNode::GetResetStateOptions() const
{
	auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this);

	if (StateLike)
	{
		return StateLike->GetEnterStates();
	}
	else
	{
		return {};
	}
}

void UAISimplePatrolNode::PostLinkerChange()
{
	Super::PostLinkerChange();
}

#endif

#undef LOCTEXT_NAMESPACE