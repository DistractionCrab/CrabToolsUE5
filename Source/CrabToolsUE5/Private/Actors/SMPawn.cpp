#include "Actors/SMPawn.h"

ASMPawn::ASMPawn(): APawn() {

	StateMachineComponent = CreateDefaultSubobject<UProcStateMachineComponent>(TEXT("StateMachineComponent"));
}



