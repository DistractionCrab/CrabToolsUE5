#pragma once

#include "CoreMinimal.h"

#include "StateMachineEnum.generated.h"

UENUM(BlueprintType)
enum class EStateMachineAccessibility : uint8
{
	/* All access available. */
	PUBLIC          UMETA(DisplayName = "Public"),
	/* For State Machines, Children can use them in Hierarchy Nodes. For Nodes, they can be transitioned to and from.  */
	PROTECTED       UMETA(DisplayName = "Protected"),
	/* Can only be seen/used in the machine that defines them. */
	PRIVATE         UMETA(DisplayName = "Private"),
	/* For State Machines, new nodes can be added. For Nodes, they can have their nodes overriden.*/
	OVERRIDEABLE    UMETA(DisplayName = "Overrideable"),
};

namespace StateMachineAccessibility
{
	bool IsChildVisible(EStateMachineAccessibility Access);
}