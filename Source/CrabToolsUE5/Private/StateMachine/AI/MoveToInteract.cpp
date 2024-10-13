#include "StateMachine/AI/MoveToInteract.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/AI/Events.h"
#include "StateMachine/AI/AIStructs.h"
#include "Components/Interaction/InteractionSystem.h"

UAIMoveToInteractNode::UAIMoveToInteractNode()
{
	this->AddEmittedEvent(AI_Events::AI_CANNOT_INTERACT);
}

void UAIMoveToInteractNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();
}

void UAIMoveToInteractNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	Super::EnterWithData_Inner_Implementation(Data);

	if (IsValid(Data))
	{
		this->SetTarget(Data);
	}
	else
	{
		this->Enter();
	}
}

void UAIMoveToInteractNode::Enter_Inner_Implementation()
{
	Super::Enter_Inner_Implementation();

	if (auto Data = this->GetMovementData())
	{
		this->SetTarget(Data->DestinationActor);
	}
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

	if (auto InteractComp = this->GetInteractionComponent())
	{
		InteractComp->OnInteractableAddedEvent.RemoveAll(this);
	}
}

bool UAIMoveToInteractNode::HandleInteraction()
{
	bool bDidInteract = false;

	if (auto InteractComp = this->GetInteractionComponent())
	{
		if (InteractComp->HasObject(this->Target))
		{
			InteractComp->Select(this->Target);
			InteractComp->Interact();

			bDidInteract = true;
		}
	}

	return bDidInteract;
}

void UAIMoveToInteractNode::SetTarget(UObject* Data)
{
	if (IsValid(Data))
	{
		if (Data->Implements<UInteractableInterface>())
		{
			if (auto Actor = Cast<AActor>(Data))
			{
				this->Target = Actor;
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
		this->EmitEvent(AI_Events::AI_CANNOT_INTERACT);
	}
	else
	{
		if (!this->Target->Implements<UInteractableInterface>())
		{
			this->EmitEvent(AI_Events::AI_CANNOT_INTERACT);
		}
		else
		{
			if (this->HandleInteraction())
			{
				this->EmitEvent(AI_Events::AI_ARRIVE);
			}
		}
	}
}

UInteractionSystem* UAIMoveToInteractNode::GetInteractionComponent() const
{
	if (auto InteractComp = this->GetOwner()->FindComponentByClass<UInteractionSystem>())
	{
		return InteractComp;
	}

	return nullptr;
}

void UAIMoveToInteractNode::BindEvents()
{
	if (auto InteractComp = this->GetInteractionComponent())
	{
		InteractComp->OnInteractableAddedEvent.AddDynamic(this, &UAIMoveToInteractNode::OnInteractableAdded);
	}
}

void UAIMoveToInteractNode::OnInteractableAdded(TScriptInterface<IInteractableInterface> Interactable)
{
	if (Interactable.GetObject() == this->Target)
	{
		this->StopMovement();
	}
}

#if WITH_EDITOR

#endif