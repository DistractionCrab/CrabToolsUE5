#pragma once

#include "CoreMinimal.h"
#include "EventListener.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UEventListenerInterface : public UInterface
{
	GENERATED_BODY()
};

class IEventListenerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="ProcStateMachine")
	void Event(FName EName);
	void Event_Implementation(FName EName) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="ProcStateMachine")
	void EventWithData(FName EName, UObject* Data);
	void EventWithData_Implementation(FName EName, UObject* Data) {}
};