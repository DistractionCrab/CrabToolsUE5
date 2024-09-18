#include "StateMachine/Emitters/EventEmitter.h"
#include "StateMachine/StateMachine.h"

void UEventEmitter::Initialize(UStateMachine* POwner)
{
	this->Owner = POwner;
	this->Initialize_Inner();
}

void UEventEmitter::EmitEvent(FName Event)
{
	if (this->Owner)
	{
		this->Owner->SendEvent(Event);
	}
}