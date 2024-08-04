#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"

FName USimpleMoveTo::ARRIVE_EVENT = "AI_ON_ARRIVE";
FName USimpleMoveTo::LOST_EVENT = "AI_ON_LOST";

USimpleMoveTo::USimpleMoveTo(): DataPropRef(nullptr)
{
	this->AddEmittedEvent(ARRIVE_EVENT);
	this->AddEmittedEvent(LOST_EVENT);
}

void USimpleMoveTo::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	auto CtrlQ = this->GetAIController();

	if (CtrlQ)
	{
		
		FAIMoveCompletedSignature::FDelegate Callback;
		Callback.BindUFunction(this, "OnMoveCompleted");
		CtrlQ->ReceiveMoveCompleted.Add(Callback);

		this->FollowComponent = CtrlQ->GetPathFollowingComponent();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SimpleMoveToNode: AIController was null."));
	}
	
	if (auto Prop = this->GetMachine()->GetClass()->FindPropertyByName(this->DestinationData))
	{
		this->DataPropRef = (FStructProperty*) Prop;
	}
}

void USimpleMoveTo::EnterWithData_Implementation(UObject* Data)
{
	if (auto Actor = Cast<AActor>(Data))
	{
		if (auto Ctrl = this->GetAIController())
		{
			Ctrl->MoveToActor(Actor);
		}
	}
}

void USimpleMoveTo::Enter_Implementation()
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

void USimpleMoveTo::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Successful pathing"));
		this->GetMachine()->Event(ARRIVE_EVENT);
	}
	else if (Result == EPathFollowingResult::Blocked)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed pathing"));
		this->GetMachine()->Event(LOST_EVENT);
	}
	
}

#if WITH_EDITORONLY_DATA
TArray<FString> USimpleMoveTo::GetPropertyOptions()
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
void USimpleMoveTo::PostLinkerChange()
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
	}

	this->DestinationData = NAME_None;
	UE_LOG(LogTemp, Warning, TEXT("SimpleMoveTo: Movement Property no longer exists."));
}
#endif