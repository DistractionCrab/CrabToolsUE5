#include "StateMachine/AI/SimplePatrolNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/AI/Events.h"
#include "StateMachine/Logging.h"

#define LOCTEXT_NAMESPACE "AISimplePatrolNode"

UAISimplePatrolNode::UAISimplePatrolNode()
{
	this->AddEmittedEvent(AI_Events::AI_ARRIVE);
	this->AddEmittedEvent(AI_Events::AI_LOST);
}

void UAISimplePatrolNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	FString Address = this->PropertyName.ToString();
	FSMPropertySearch Params = FSMPropertySearch::StructProperty(FPatrolPathState::StaticStruct());
	this->PropertyRef = Params.GetProperty<FStructProperty>(this->GetMachine(), Address);

	check(this->PropertyRef);
}

void UAISimplePatrolNode::Enter_Inner_Implementation()
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

	if (auto Ctrl = this->GetAIController())
	{
		Ctrl->StopMovement();
	}
}

void UAISimplePatrolNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	this->GetState()->Step();

	if (Result == EPathFollowingResult::Success)
	{
		this->EmitEvent(AI_Events::AI_ARRIVE);
		if (this->Active())
		{
			this->MoveToNext();
		}
	}
	else if (Result == EPathFollowingResult::Blocked)
	{
		this->MoveToNext();
	}
}

void UAISimplePatrolNode::MoveToNext()
{
	auto State = this->GetState();

	if (!IsValid(State->GetPath())) { return; }

	if (auto Ctrl = this->GetAIController())
	{
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
}

FPatrolPathState* UAISimplePatrolNode::GetState() const
{
	auto State = this->PropertyRef.GetValue<FPatrolPathState>();

	check(State);

	return State;
}

void UAISimplePatrolNode::BindCallback()
{
	if (auto CtrlQ = this->GetAIController())
	{
		FAIMoveCompletedSignature::FDelegate Callback;
		//Callback.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UAISimplePatrolNode, OnMoveCompleted));
		//CtrlQ->ReceiveMoveCompleted.Add(Callback);
		CtrlQ->ReceiveMoveCompleted.AddDynamic(this, &UAISimplePatrolNode::OnMoveCompleted);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AISimplePatrolNode: AIController was null."));
	}
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