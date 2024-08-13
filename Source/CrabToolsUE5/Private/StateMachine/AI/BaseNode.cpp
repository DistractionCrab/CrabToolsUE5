#include "StateMachine/AI/BaseNode.h"
#include "AIController.h"

void UAIBaseNode::Initialize_Implementation() {
	auto PawnQ = Cast<APawn>(this->GetMachine()->GetOwner());

	if (PawnQ) {
		this->PawnRef = PawnQ;

		this->AICtrl = Cast<AAIController>(PawnQ->GetController());
	}
}