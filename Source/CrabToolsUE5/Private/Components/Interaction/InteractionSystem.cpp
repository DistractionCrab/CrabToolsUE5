
#include "Components/Interaction/InteractionSystem.h"

UInteractionSystem::UInteractionSystem() {
	this->SelectedIndex = 0;
	this->PrimaryComponentTick.bCanEverTick = true;
}


void UInteractionSystem::AddInteractable(UObject* Obj) {
	if (Obj && Obj->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass())) {
		this->InteractableObjects.Add(Obj);
		if (this->InteractableObjects.Num() == 1) {
			TScriptInterface<IInteractableInterface> IFace = Obj;
			this->InteractableUpdatedEvent.Broadcast(IFace);
		}
	}
}


void UInteractionSystem::RemoveInteractable(UObject* Obj) {

	this->InteractableObjects.Remove(Obj);
	this->SelectedIndex = 0;

	if (this->InteractableObjects.Num() > 0) {
		TScriptInterface<IInteractableInterface> IFace = Obj;
		this->InteractableUpdatedEvent.Broadcast(IFace);
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

	TScriptInterface<IInteractableInterface> IFace = this->InteractableObjects[this->SelectedIndex].Get();
	this->InteractableUpdatedEvent.Broadcast(IFace);
}

TScriptInterface<IInteractableInterface> UInteractionSystem::GetSelected() {
	if (this->InteractableObjects.Num() > 0) {
		TScriptInterface<IInteractableInterface> IFace = this->InteractableObjects[this->SelectedIndex].Get();
		return IFace;
	}

	return nullptr;
}