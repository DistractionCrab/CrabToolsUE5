#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Utils/Enums.h"
#include "Navigation/NavLinkProxy.h"
#include "InteractNavLinkProxy.generated.h"


UINTERFACE(Blueprintable, MinimalAPI)
class UInteractableNavigationInterface: public UInterface 
{
	GENERATED_BODY()

};

class CRABTOOLSUE5_API IInteractableNavigationInterface 
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="AI|Navigation",
		meta=(ExpandEnumAsExecs="Result"))
	AActor* GetPathActor(ESearchResult& Result);
	virtual AActor* GetPathActor_Implementation(ESearchResult& Result) { 
		Result = ESearchResult::NotFound;
		return nullptr; 
	}
};

UCLASS(Blueprintable, Category="AI")
class CRABTOOLSUE5_API AInteractNavLinkProxy: public ANavLinkProxy, public IInteractableNavigationInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="AI|Navigation", meta=(AllowPrivateAccess=true))
	TObjectPtr<AActor> InteractActor;

public:
	virtual AActor* GetPathActor_Implementation(ESearchResult& Result) override {
		if (this->InteractActor == nullptr) {
			Result = ESearchResult::NotFound;
		} else {
			Result = ESearchResult::Found;
		}
		return InteractActor; 
	}
};