#include "StateMachine/PlaceholderNode.h"

void UPlaceholderNode::Initialize_Implementation()
{
	this->SubNode = *this->GetMachine()->GetSharedNodes().Find(this->SlotName);
}