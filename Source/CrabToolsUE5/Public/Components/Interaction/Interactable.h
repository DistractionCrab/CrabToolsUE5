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
	virtual void InteractWithData_Implementation(AActor* User, UObject* Data) {  }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction",
		meta=(ForceAsFunction))
	void GetLocations(UPARAM(ref) TArray<FVector>& Locations);
	virtual void GetLocations_Implementation(UPARAM(ref) TArray<FVector>& Locations)
	{
		if (auto Actor = Cast<AActor>(this))
		{
			Locations.Add(Actor->GetActorLocation());
		}
	}
};