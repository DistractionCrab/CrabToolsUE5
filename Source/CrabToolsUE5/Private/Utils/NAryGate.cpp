#include "Utils/NAryGate.h"

ETriggerBranch FNAryGate::Activate() 
{

	this->CurrentCount += 1;

	if (this->CurrentCount == this->CountGate)
	{
		return ETriggerBranch::Trigger;
	} 
	else 
	{
		return ETriggerBranch::Null;
	}
}

ETriggerBranch FNAryGate::Deactivate() 
{
	if (this->CurrentCount == 0) 
	{
		return ETriggerBranch::Null;
	}

	if (this->CurrentCount == this->CountGate)
	{
		this->CurrentCount -= 1;
		return ETriggerBranch::Trigger;
	} 
	else 
	{
		this->CurrentCount -= 1;
		return ETriggerBranch::Null;
	}
}