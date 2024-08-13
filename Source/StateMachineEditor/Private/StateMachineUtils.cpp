#include "StateMachineUtils.h"


void UStateMachineUtils::AddEventsToEventSet(UStateMachineBlueprint* BP, UDataTable* EventSet, bool bClearEntries)
{
	BP->AddEventsToDataTable(EventSet, bClearEntries);
}