
#include "Components/Interaction/InteractionSystem.h"

UInteractionSystem::UInteractionSystem() {
	this->SelectedIndex = 0;
	this->PrimaryComponentTick.bCanEverTick = true;
}


void UInteractionSystem::AddInteractable(UObject* Obj) {
	if (Obj && Obj->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass())) {
		this->InteractableObjects.Add(Obj);

		TScriptInterface<IInteractableInterface> IFace = Obj;
		this->InteractableAddedEvent.Broadcast(IFace);
		if (this->InteractableObjects.Num() == 1) {
			this->InteractableSelectedEvent.Broadcast(IFace);
		}
	}
}


void UInteractionSystem::RemoveInteractable(UObject* Obj) {

	this->InteractableObjects.Remove(Obj);
	this->SelectedIndex = 0;

	if (Obj && Obj->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass())) {
		TScriptInterface<IInteractableInterface> IFace = Obj;
		this->InteractableRemovedEvent.Broadcast(IFace);

		if (this->InteractableObjects.Num() > 0) {
			TScriptInterface<IInteractableInterface> NewIFace = this->InteractableObjects[0].Get();
			this->InteractableSelectedEvent.Broadcast(NewIFace);
		}
	}
}


void UInteractionSystem::Interact() {
	this->InteractWith(this->GetOwner());
}


void UInteractionSystem::InteractWith(AActor* Redirect) {
	if (this->InteractableObjects.Num() > 0) {
		auto Obj = this->InteractableObjects[this->SelectedIndex];

		if (Obj.IsValid()) {
			IInteractableInterface::Execute_Interact(Obj.Get(), Redirect);
		}
		else {
			this->InteractableObjects.RemoveAt(this->SelectedIndex);
		}
	}
}


void UInteractionSystem::Cycle() {
	if (this->InteractableObjects.Num() == 0) { return; }
	this->SelectedIndex = (this->SelectedIndex + 1) % this->InteractableObjects.Num();

	TScriptInterface<IInteractableInterface> NewIFace = this->InteractableObjects[this->SelectedIndex].Get();
	this->InteractableSelectedEvent.Broadcast(NewIFace);

	//TScriptInterface<IInteractableInterface> IFace = this->InteractableObjects[this->SelectedIndex].Get();
	//this->InteractableUpdatedEvent.Broadcast(IFace);
}

TScriptInterface<IInteractableInterface> UInteractionSystem::GetSelected() {
	if (this->InteractableObjects.Num() > 0) {
		TScriptInterface<IInteractableInterface> IFace = this->InteractableObjects[this->SelectedIndex].Get();
		return IFace;
	}

	return nullptr;
}

int UInteractionSystem::IndexOf(UObject* Obj) {
	return this->InteractableObjects.IndexOfByPredicate([&](const TWeakObjectPtr<UObject> TObj) {
		return 	TObj.IsValid() && TObj.Get() == Obj;
	});
}