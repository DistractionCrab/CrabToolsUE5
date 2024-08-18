#include "Actors/SMCharacter.h"

ASMCharacter::ASMCharacter(): ACharacter()
{
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachineComponent"));
}



