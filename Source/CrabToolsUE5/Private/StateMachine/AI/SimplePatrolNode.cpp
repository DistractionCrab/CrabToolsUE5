#include "StateMachine/AI/SimplePatrolNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/AI/Events.h"

#define LOCTEXT_NAMESPACE "AISimplePatrolNode"

UAISimplePatrolNode::UAISimplePatrolNode(): PatrolProperty(nullptr)
{
	this->AddEmittedEvent(AI_Events::AI_ARRIVE);
	this->AddEmittedEvent(AI_Events::AI_LOST);
}

void UAISimplePatrolNode::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	FString Address = this->PatrolPathProperty.ToString();

	if (auto Prop = this->GetMachine()->GetStateMachineProperty(Address))
	{
		this->PatrolProperty = (FObjectProperty*) Prop;
	}

	this->PatrolState.Reset();
}

void UAISimplePatrolNode::Enter_Implementation()
{
	if (!this->NonResetStates.Contains(this->GetMachine()->GetPreviousState()))
	{
		this->PatrolState.Reset();
	}

	this->BindCallback();	
	this->MoveToNext();
}

void UAISimplePatrolNode::Exit_Implementation()
{
	this->UnbindCallback();

	if (auto Ctrl = this->GetAIController())
	{
		Ctrl->StopMovement();
	}
}

void UAISimplePatrolNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	this->PatrolState.Skip();

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
	if (this->PatrolProperty)
	{
		auto PatrolPath = *this->PatrolProperty->ContainerPtrToValuePtr<APatrolPath*>(this->GetMachine());		

		if (PatrolPath == nullptr || !IsValid(PatrolPath)) { return; }

		if (auto Ctrl = this->GetAIController())
		{
			FVector Goal = this->PatrolState.GetCurrentTarget(PatrolPath);
			this->RecurseGuard += 1;

			if (this->RecurseGuard > PatrolPath->Num())
			{
				// If we've recursed too many times, then remove the call back.
				this->GetAIController()->ReceiveMoveCompleted.RemoveAll(this);
			}

			Ctrl->MoveToLocation(Goal);

			this->RecurseGuard = 0;
		}
	}
}

void UAISimplePatrolNode::BindCallback()
{
	if (auto CtrlQ = this->GetAIController())
	{
		FAIMoveCompletedSignature::FDelegate Callback;
		Callback.BindUFunction(this, "OnMoveCompleted");
		CtrlQ->ReceiveMoveCompleted.Add(Callback);
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
		FSMPropertySearch Params;

		Params.FClass = FObjectProperty::StaticClass();
		Params.Class = APatrolPath::StaticClass();

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

	if (this->PatrolPathProperty != NAME_None)
	{
		// Otherwise, make sure the property exists, and is of the correct type.
		if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
		{
			FString Address = this->PatrolPathProperty.ToString();

			if (Outer->GetStateMachineProperty(Address))
			{
				return;
			}
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE