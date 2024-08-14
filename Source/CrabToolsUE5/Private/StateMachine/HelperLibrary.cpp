#include "StateMachine/HelperLibrary.h"
#include "StateMachine/EventListener.h"

void UStateMachineHelperLibrary::EmitEvent(UObject* Obj, FName EName)
{
	IEventListenerInterface::Execute_Event(Obj, EName);
}

void UStateMachineHelperLibrary::EmitEventWithData(UObject* Obj, FName EName, UObject* Data)
{
	IEventListenerInterface::Execute_EventWithData(Obj, EName, Data);
}