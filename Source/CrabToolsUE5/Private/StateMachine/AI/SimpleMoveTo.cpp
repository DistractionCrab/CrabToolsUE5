#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/AI/Events.h"

UAISimpleMoveToNode::UAISimpleMoveToNode(): DataPropRef(nullptr)
{
	this->AddEmittedEvent(AI_Events::AI_ARRIVE);
	this->AddEmittedEvent(AI_Events::AI_LOST);
}

void UAISimpleMoveToNode::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	FString Address = this->DestinationData.ToString();

	if (auto Prop = this->GetMachine()->GetStateMachineProperty(Address))
	{
		this->DataPropRef = (FStructProperty*) Prop;
	}
}

void UAISimpleMoveToNode::Exit_Implementation()
{
	this->UnbindCallback();

	if (auto Ctrl = this->GetAIController())
	{
		Ctrl->StopMovement();
	}
}

void UAISimpleMoveToNode::EnterWithData_Implementation(UObject* Data)
{
	this->BindCallback();
	this->MoveTo(Data);
}

void UAISimpleMoveToNode::EventWithData_Implementation(FName EName, UObject* Data)
{
	this->MoveTo(Data);
}

void UAISimpleMoveToNode::MoveTo(UObject* Data)
{
	if (auto Actor = Cast<AActor>(Data))
	{
		if (auto Ctrl = this->GetAIController())
		{
			if (Ctrl->IsFollowingAPath())
			{
				Ctrl->StopMovement();
			}

			Ctrl->MoveToActor(Actor);
		}
		else
		{
			this->EmitEvent(AI_Events::AI_LOST);
		}
	}
	else
	{
		this->EmitEvent(AI_Events::AI_LOST);
	}
}

void UAISimpleMoveToNode::Enter_Implementation()
{
	this->BindCallback();
	if (this->DataPropRef)
	{
		auto Value = this->DataPropRef->ContainerPtrToValuePtr<FMoveToData>(this->GetMachine());

		if (auto Ctrl = this->GetAIController())
		{
			if (Value->DestinationActor)
			{
				Ctrl->MoveToActor(Value->DestinationActor);
			}
			else
			{
				Ctrl->MoveToLocation(Value->DestinationLocation);
			}
		}
	}
}

void UAISimpleMoveToNode::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		this->EmitEvent(AI_Events::AI_ARRIVE);
	}
	else if (Result == EPathFollowingResult::Blocked)
	{
		this->EmitEvent(AI_Events::AI_LOST);
	}
	
}

void UAISimpleMoveToNode::BindCallback()
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

void UAISimpleMoveToNode::UnbindCallback()
{
	if (auto CtrlQ = this->GetAIController())
	{
		CtrlQ->ReceiveMoveCompleted.RemoveAll(this);
	}
}

#if WITH_EDITOR
TArray<FString> UAISimpleMoveToNode::GetPropertyOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		FSMPropertySearch Params;

		Params.FClass = FStructProperty::StaticClass();
		Params.Struct = FMoveToData::StaticStruct();

		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}
#endif

#if WITH_EDITOR
void UAISimpleMoveToNode::PostLinkerChange()
{
	Super::PostLinkerChange();

	if (this->DestinationData != NAME_None)
	{
		// Otherwise, make sure the property exists, and is of the correct type.
		if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
		{
			FString Address = this->DestinationData.ToString();

			if (Outer->GetStateMachineProperty(Address))
			{
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("SimpleMoveTo: Movement Property no longer exists: %s"),
				*this->DestinationData.ToString());
			this->DestinationData = NAME_None;
		}
	}
}
#endif