#include "StateMachine/StateMachineEnum.h"


bool StateMachineAccessibility::IsChildVisible(EStateMachineAccessibility Access)
{
	return Access == EStateMachineAccessibility::PUBLIC ||
		Access == EStateMachineAccessibility::VISIBLE_ANYWHERE ||
		Access == EStateMachineAccessibility::PROTECTED ||
		Access == EStateMachineAccessibility::OVERRIDEABLE;
}

bool StateMachineAccessibility::IsPublic(EStateMachineAccessibility Access)
{
	return Access == EStateMachineAccessibility::PUBLIC;
}

bool StateMachineAccessibility::IsExtendible(EStateMachineAccessibility Access)
{
	return Access == EStateMachineAccessibility::PUBLIC ||
		Access == EStateMachineAccessibility::PROTECTED ||
		Access == EStateMachineAccessibility::OVERRIDEABLE;
}

bool StateMachineAccessibility::IsOverrideable(EStateMachineAccessibility Access)
{
	return Access == EStateMachineAccessibility::PUBLIC ||
		Access == EStateMachineAccessibility::OVERRIDEABLE;
}