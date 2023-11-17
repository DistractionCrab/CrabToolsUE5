#pragma once

#include "CoreMinimal.h"
#include "LockOnInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ULockOnInterface : public UInterface
{
	GENERATED_BODY()
};

class ILockOnInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Interaction")
	bool ActivateWidget();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Interaction")
	bool DeactivateWidget();
};