#include "StateMachine/EdGraph/EdStartStateNode.h"


UEdStartStateNode::UEdStartStateNode() {
	this->bCanRenameNode = false;
	this->Description = FText::FromString("Starting point for the State Machine.");
}

UEdStartStateNode::~UEdStartStateNode() {

}
