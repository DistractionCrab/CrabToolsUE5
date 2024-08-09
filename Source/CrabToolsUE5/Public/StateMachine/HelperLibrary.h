#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateMachine/EventListener.h"
#include "HelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CRABTOOLSUE5_API UStateMachineHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	

	UFUNCTION(BlueprintCallable, Category="Utility")
	static void EmitEvent(UObject* Obj, FName EName);

	UFUNCTION(BlueprintCallable, Category="Utility")
	static void EmitEventWithData(UObject* Obj, FName EName, UObject* Data);
};
