#include "StateMachine/AI/SimplePatrolNode.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"

#define LOCTEXT_NAMESPACE "AISimplePatrolNode"

void UAISimplePatrolNode::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	auto CtrlQ = this->GetAIController();

	if (CtrlQ)
	{
		FAIMoveCompletedSignature::FDelegate Callback;
		Callback.BindUFunction(this, "OnMoveCompleted");
		CtrlQ->ReceiveMoveCompleted.Add(Callback);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AISimplePatrolNode: AIController was null."));
	}

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
	this->MoveToNext();
}

void UAISimplePatrolNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		this->MoveToNext();
	}
	else if (Result == EPathFollowingResult::Blocked)
	{
		
	}
}

void UAISimplePatrolNode::MoveToNext()
{
	this->PatrolState.Reset();

	if (this->PatrolProperty)
	{
		auto Value = this->PatrolProperty->ContainerPtrToValuePtr<APatrolPath*>(this->GetMachine());

		if (auto Ctrl = this->GetAIController())
		{
			if (auto Goal = this->PatrolState.GetNextTarget(*Value))
			{
				Ctrl->MoveToActor(Goal);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Found null actor while patrolling. Cannot proceed."));
			}
		}
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
#endif

#undef LOCTEXT_NAMESPACE