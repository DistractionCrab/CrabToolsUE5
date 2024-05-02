#include "StateMachine/Schema/GraphSchema.h"
#include "StateMachine/StateMachineBlueprint.h"

UGraphSchema::UGraphSchema(
	const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


void UGraphSchema::BackwardCompatibilityNodeConversion(UEdGraph* Graph, bool bOnlySafeChanges) const
{
	if (Graph)
	{
		if (UStateMachineBlueprint* ProcStateMachineBlueprint = Cast<UStateMachineBlueprint>(Graph->GetOuter()))
		{
			const int32 ProcStateMachineBPVersion = ProcStateMachineBlueprint->GetLinkerCustomVersion(FFortniteMainBranchObjectVersion::GUID);
		}
	}

	Super::BackwardCompatibilityNodeConversion(Graph, bOnlySafeChanges);
}