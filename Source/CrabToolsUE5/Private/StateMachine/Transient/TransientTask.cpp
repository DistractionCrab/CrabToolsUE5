#include "StateMachine/Transient/TransientTask.h"

void UTransientTask::TaskCompleted()
{
	this->OnTaskCompleted.Broadcast();
}