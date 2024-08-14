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

	auto CtrlQ = this->GetAIController();

	if (CtrlQ)
	{
		FAIMoveCompletedSignature::FDelegate Callback;
		Callback.BindUFunction(this, "OnMoveCompleted");
		CtrlQ->ReceiveMoveCompleted.Add(Callback);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AISimpleMoveToNode: AIController was null."));
	}
	
	if (auto Prop = this->GetMachine()->GetClass()->FindPropertyByName(this->DestinationData))
	{
		if (Prop->GetClass() == FStructProperty::StaticClass())
		{
			//this->DataPropRef = Cast<FStructProperty>(Prop);
			this->DataPropRef = (FStructProperty*) Prop;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid property found for movement information. Found Type: %s"),
				*Prop->GetClass()->GetName());
		}
	}
}

void UAISimpleMoveToNode::EnterWithData_Implementation(UObject* Data)
{
	if (auto Actor = Cast<AActor>(Data))
	{
		if (auto Ctrl = this->GetAIController())
		{
			Ctrl->MoveToActor(Actor);
		}
	}
}

void UAISimpleMoveToNode::Enter_Implementation()
{
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

#if WITH_EDITOR
TArray<FString> UAISimpleMoveToNode::GetPropertyOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		if (auto SMClass = Outer->GetStateMachineClass())
		{
			for (TFieldIterator<FStructProperty> FIT(SMClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
			{
				FStructProperty* f = *FIT;

				if (f->Struct == FMoveToData::StaticStruct())
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

#if WITH_EDITOR
void UAISimpleMoveToNode::PostLinkerChange()
{
	Super::PostLinkerChange();

	// If it was never set, then don't bother checking.
	if (this->DestinationData == NAME_None) { return; }

	// Otherwise, make sure the property exists, and is of the correct type.
	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		if (auto SMClass = Outer->GetStateMachineClass())
		{
			for (TFieldIterator<FStructProperty> FIT(SMClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
			{
				FStructProperty* f = *FIT;

				if (f->Struct == FMoveToData::StaticStruct() && f->GetFName() == this->DestinationData)
				{
					return;
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("SimpleMoveTo: Movement Property no longer exists: %s"),
			*this->DestinationData.ToString());
		this->DestinationData = NAME_None;		
	}	
}
#endif