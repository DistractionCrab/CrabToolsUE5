#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Components/Interaction/Interactable.h"
#include "InteractionSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractionSelected, TScriptInterface<IInteractableInterface>, Selected);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRABTOOLSUE5_API UInteractionSystem : public UActorComponent
{
	GENERATED_BODY()

	
	TArray<TWeakObjectPtr<UObject>> InteractableObjects;
	unsigned int SelectedIndex;

public:

	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FInteractionSelected InteractableAddedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionSelected InteractableRemovedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FInteractionSelected InteractableSelectedEvent;

public:	
	// Sets default values for this component's properties
	UInteractionSystem();


public:	
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void AddInteractable(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void RemoveInteractable(UObject* Obj);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Interact();

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void InteractWith(AActor* Redirect);
	

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Cycle();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	int Num() { return this->InteractableObjects.Num(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	UObject* GetObj(int index) { return index >= 0 && index < this->InteractableObjects.Num() ? this->InteractableObjects[index].Get() : nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction")
	int IndexOf(UObject* Obj);

	TScriptInterface<IInteractableInterface> GetSelected();
};
