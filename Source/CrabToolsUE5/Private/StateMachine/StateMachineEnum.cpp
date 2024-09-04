#include "StateMachine/StateMachineEnum.h"


bool StateMachineAccessibility::IsChildVisible(EStateMachineAccessibility Access)
{
	return Access == EStateMachineAccessibility::PUBLIC ||
		Access == EStateMachineAccessibility::PROTECTED ||
		Access == EStateMachineAccessibility::OVERRIDEABLE;
}

bool StateMachineAccessibility::IsPublic(EStateMachineAccessibility Access)
{
	return Access == EStateMachineAccessibility::PUBLIC;
}