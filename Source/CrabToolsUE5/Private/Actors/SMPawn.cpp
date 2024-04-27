#include "Actors/SMPawn.h"

ASMPawn::ASMPawn(): APawn() {

	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachineComponent"));
}



