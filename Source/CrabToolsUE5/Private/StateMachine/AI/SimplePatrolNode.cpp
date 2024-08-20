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

	if (auto Prop = this->GetMachine()->GetClass()->FindPropertyByName(this->PatrolPathProperty))
	{
		if (Prop->GetClass() == FObjectProperty::StaticClass())
		{
			this->PatrolProperty = (FObjectProperty*) Prop;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid property found for patrol path. Found Type: %s"),
				*Prop->GetClass()->GetName());
		}
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
		if (auto SMClass = Outer->GetStateMachineClass())
		{
			for (TFieldIterator<FObjectProperty> FIT(SMClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
			{
				FObjectProperty* f = *FIT;

				if (f->PropertyClass == APatrolPath::StaticClass())
				{
					Props.Add(f->GetName());
				}
			}
		}
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

#endif

#undef LOCTEXT_NAMESPACE