#include "StateMachine/AI/AISimplePatrolNode.h"
#include "AIController.h"

void UAISimplePatrolNode::Initialize_Implementation() {
	Super::Initialize_Implementation();

	auto CtrlQ = this->GetAIController();

	if (CtrlQ) {
		//this->FollowComponent = CtrlQ->GetPathFollowingComponent();
	}	
}