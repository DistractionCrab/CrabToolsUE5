#include "StateMachine/PlaceholderNode.h"

UStateNode* UPlaceholderNode::Substitute(FName SlotNameSearch, UStateNode* Node) {
	if (this->SlotName == SlotNameSearch) {
		return Node;
	} else {
		return this;
	}	
}