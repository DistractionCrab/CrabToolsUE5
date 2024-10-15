#include "StateMachine/AI/MoveToInteract.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/Events.h"
#include "StateMachine/AI/AIStructs.h"
#include "Components/Interaction/InteractionSystem.h"
#include "Utils/PathFindingUtils.h"

UAIMoveToInteractNode::UAIMoveToInteractNode()
{
	this->AddEmittedEvent(Events::AI::CANNOT_INTERACT);
}

void UAIMoveToInteractNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	check(IsValid(this->GetInteractionComponent()));
}

void UAIMoveToInteractNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (IsValid(Data))
	{
		this->SetTarget(Data);
		this->BindEvents();

		Super::EnterWithData_Inner_Implementation(Data);
	}
	else
	{
		this->Enter();
	}
}

void UAIMoveToInteractNode::Enter_Inner_Implementation()
{
	if (auto Data = this->GetMovementData())
	{
		this->SetTarget(Data->DestinationActor);
	}

	this->BindEvents();

	Super::Enter_Inner_Implementation();
}

void UAIMoveToInteractNode::Exit_Inner_Implementation()
{
	Super::Exit_Inner_Implementation();

	if (this->GetMovementResult() == EPathFollowingResult::Success)
	{
		this->HandleInteraction();
	}
	else if (this->GetMovementResult() == EPathFollowingResult::Aborted)
	{
		this->HandleInteraction();
	}

	this->GetInteractionComponent()->OnInteractableAddedEvent.RemoveAll(this);
}

bool UAIMoveToInteractNode::HandleInteraction()
{
	bool bDidInteract = false;

	auto InteractComp = this->GetInteractionComponent();

	if (InteractComp->HasObject(this->Target))
	{
		InteractComp->Select(this->Target);
		InteractComp->Interact();

		bDidInteract = true;
	}

	return bDidInteract;
}

bool UAIMoveToInteractNode::HasInteractable() const
{
	return this->GetInteractionComponent()->HasObject(this->Target);
}

void UAIMoveToInteractNode::SetTarget(UObject* Data)
{
	if (IsValid(Data))
	{
		if (Data->Implements<UInteractableInterface>())
		{
			this->Target = Data;

			TArray<FVector> Locations;
			IInteractableInterface::Execute_GetLocations(Data, Locations);

			if (Locations.Num() > 0)
			{
				auto Dest = UPathFindingUtilsLibrary::ChooseNearLocation(this->GetAIController(), Locations);

				this->SetOverrideLocation(Dest);
			}
		}
		else
		{
			this->Target = nullptr;
		}
	}
}

void UAIMoveToInteractNode::PostTransition_Inner_Implementation()
{
	if (!IsValid(this->Target))
	{
		this->EmitEvent(Events::AI::CANNOT_INTERACT);
	}
	else
	{
		if (!this->Target->Implements<UInteractableInterface>())
		{
			this->EmitEvent(Events::AI::CANNOT_INTERACT);
		}
		else
		{
			if (this->HasInteractable())
			{
				this->EmitEvent(Events::AI::ARRIVE);
			}
		}
	}
}

UInteractionSystem* UAIMoveToInteractNode::GetInteractionComponent() const
{
	auto InteractComp = this->GetOwner()->FindComponentByClass<UInteractionSystem>();
	
	return InteractComp;
}

void UAIMoveToInteractNode::BindEvents()
{
	this->GetInteractionComponent()->OnInteractableAddedEvent.AddDynamic(this, &UAIMoveToInteractNode::OnInteractableAdded);
}

void UAIMoveToInteractNode::OnInteractableAdded(TScriptInterface<IInteractableInterface> Interactable)
{
	if (Interactable.GetObject() == this->Target)
	{
		this->EmitEvent(Events::AI::ARRIVE);
	}
}

#if WITH_EDITOR

#endif