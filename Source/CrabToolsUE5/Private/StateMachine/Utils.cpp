#include "StateMachine/Utils.h"
#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"

FName Naming::GenerateStateNodeName(UStateMachine* Outer, FName StateName)
{
	FString NameBase = "";
	NameBase.Append(Outer->GetName());
	NameBase.Append("::");
	NameBase.Append(StateName.ToString());

	return FName(NameBase);
}

FName Naming::GenerateStateNodeName(UStateNode* Node, FName StateName)
{
	return FName(FString::Printf(TEXT("%s(%s)"), *Node->GetName(), *StateName.ToString()));
}