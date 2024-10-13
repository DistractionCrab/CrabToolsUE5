#pragma once

#include "CoreMinimal.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interaction")
	void Interact(AActor* User);
	virtual void Interact_Implementation(AActor* User) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void InteractWithData(AActor* User, UObject* Data);
	virtual void InteractWithData_Implementation(AActor* User, UObject* Data) {}
};