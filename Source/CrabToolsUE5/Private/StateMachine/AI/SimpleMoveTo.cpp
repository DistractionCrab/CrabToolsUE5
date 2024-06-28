#include "StateMachine/AI/SimpleMoveTo.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

USimpleMoveTo::USimpleMoveTo()
: OnLostEvent(LOST_EVENT), 
	OnArriveEvent(ARRIVE_EVENT) 
{

}

void USimpleMoveTo::Initialize_Implementation()
{
	Super::Initialize_Implementation();

	auto CtrlQ = this->GetAIController();

	if (CtrlQ)
	{
		this->FollowComponent = CtrlQ->GetPathFollowingComponent();
	}	
}

void USimpleMoveTo::EnterWithData_Implementation(UObject* Data)
{

}