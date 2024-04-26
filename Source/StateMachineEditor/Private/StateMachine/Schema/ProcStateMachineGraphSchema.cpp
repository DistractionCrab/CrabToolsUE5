#include "StateMachine/Schema/ProcStateMachineGraphSchema.h"

UProcStateMachineGraphSchema::UProcStateMachineGraphSchema(
	const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


void UProcStateMachineGraphSchema::BackwardCompatibilityNodeConversion(UEdGraph* Graph, bool bOnlySafeChanges) const
{
	if (Graph)
	{
		if (UProcStateMachineBlueprint* ProcStateMachineBlueprint = Cast<UProcStateMachineBlueprint>(Graph->GetOuter()))
		{
			const int32 ProcStateMachineBPVersion = ProcStateMachineBlueprint->GetLinkerCustomVersion(FFortniteMainBranchObjectVersion::GUID);
		}
	}

	Super::BackwardCompatibilityNodeConversion(Graph, bOnlySafeChanges);
}