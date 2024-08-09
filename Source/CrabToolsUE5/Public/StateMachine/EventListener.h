#pragma once

#include "CoreMinimal.h"
#include "EventListener.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UEventListenerInterface : public UInterface
{
	GENERATED_BODY()
};

class IEventListenerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="StateMachine")
	void Event(FName EName);
	virtual void Event_Implementation(FName EName) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="StateMachine")
	void EventWithData(FName EName, UObject* Data);
	virtual void EventWithData_Implementation(FName EName, UObject* Data) {}
};