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
	FInteractionSelected InteractableUpdatedEvent;

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

	TScriptInterface<IInteractableInterface> GetSelected();
};
