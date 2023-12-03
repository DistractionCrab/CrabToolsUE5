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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="LockOnCamera")
	void ActivateWidget();
	virtual void ActivateWidget_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="LockOnCamera")
	void DeactivateWidget();
	virtual void DeactivateImplementation() {}
};