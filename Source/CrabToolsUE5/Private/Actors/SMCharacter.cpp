#include "Actors/SMCharacter.h"

ASMCharacter::ASMCharacter(): ACharacter() {

	StateMachineComponent = CreateDefaultSubobject<UProcStateMachineComponent>(TEXT("StateMachineComponent"));
}



