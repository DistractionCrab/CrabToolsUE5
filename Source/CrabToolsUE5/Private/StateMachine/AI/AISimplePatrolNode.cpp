#include "StateMachine/AI/AISimplePatrolNode.h"
#include "AIController.h"

void UAISimplePatrolNode::Initialize_Implementation(UProcStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);

	auto CtrlQ = this->GetAIController();

	if (CtrlQ) {
		//this->FollowComponent = CtrlQ->GetPathFollowingComponent();
	}	
}