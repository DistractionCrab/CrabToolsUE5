#include "StateMachine/AI/AIBaseNode.h"

void UAIBaseNode::Initialize_Implementation(UProcStateMachine* POwner) {
	auto PawnQ = Cast<APawn>(POwner->GetOwner());

	if (PawnQ) {
		this->PawnRef = PawnQ;

		this->AICtrl = Cast<AAIController>(PawnQ->GetController());
	}
}