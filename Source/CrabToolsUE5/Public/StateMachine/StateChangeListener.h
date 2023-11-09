#pragma once

#include "CoreMinimal.h"
#include "StateChangeListener.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UStateChangeListenerInterface : public UInterface
{
	GENERATED_BODY()
};

class IStateChangeListenerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Interaction")
    bool Listen(FName From, FName To);
};