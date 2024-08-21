#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StateMachineUtils.generated.h"

class UStateMachineBlueprint;
class UDataTable;

/**
 * 
 */
UCLASS()
class UStateMachineUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	

	UFUNCTION(BlueprintCallable, Category="StateMachine|Utility")
	static void AddEventsToEventSet(UStateMachineBlueprint* BP, UDataTable* EventSet, bool bClearEntries=false);


	UFUNCTION(BlueprintCallable, Category = "StateMachine|Utility")
	static void AddStateClassesToSet(UStateMachineBlueprint* BP, UDataTable* EventSet, bool bClearEntries = false);
};

