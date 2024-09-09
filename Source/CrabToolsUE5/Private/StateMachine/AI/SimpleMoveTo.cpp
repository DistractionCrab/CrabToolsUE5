#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/AI/Events.h"

UAISimpleMoveToNode::UAISimpleMoveToNode()
{
	this->AddEmittedEvent(AI_Events::AI_ARRIVE);
	this->AddEmittedEvent(AI_Events::AI_LOST);
}

void UAISimpleMoveToNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	FString Address = this->PropertyName.ToString();
	FSMPropertySearch Params = FSMPropertySearch::StructProperty(FMoveToData::StaticStruct());
	this->PropertyRef = Params.GetProperty<FStructProperty>(this->GetMachine(), Address);
}

void UAISimpleMoveToNode::Exit_Inner_Implementation()
{
	this->UnbindCallback();

	if (auto Ctrl = this->GetAIController())
	{
		Ctrl->StopMovement();
	}
}

void UAISimpleMoveToNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	this->BindCallback();
	this->MoveTo(Data);
}

void UAISimpleMoveToNode::EventWithData_Inner_Implementation(FName EName, UObject* Data)
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

void UAISimpleMoveToNode::Enter_Inner_Implementation()
{
	this->BindCallback();
	if (auto Value = this->PropertyRef.GetValue<FMoveToData>())
	{	
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
		FSMPropertySearch Params = FSMPropertySearch::StructProperty(FMoveToData::StaticStruct());

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
}
#endif