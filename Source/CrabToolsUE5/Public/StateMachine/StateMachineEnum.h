#pragma once

#include "CoreMinimal.h"

#include "StateMachineEnum.generated.h"

UENUM(BlueprintType)
enum class EStateMachineAccessibility : uint8
{
	/* All access available. */
	PUBLIC          UMETA(DisplayName = "Public"),
	/* Visible anywhere, but cannot be overwritten or extended in children. */
	VISIBLE_ANYWHERE    UMETA(DisplayName = "VisibleAnywhere"),
	/* For State Machines, Children can use them in Hierarchy Nodes. For Nodes, they can be transitioned to and from.  */
	PROTECTED       UMETA(DisplayName = "Protected"),
	/* Can only be seen/used in the machine that defines them. */
	PRIVATE         UMETA(DisplayName = "Private"),
	/* For State Machines, new nodes can be added. For Nodes, they can have their data overriden.*/
	OVERRIDEABLE    UMETA(DisplayName = "Overrideable"),

};

namespace StateMachineAccessibility
{
	CRABTOOLSUE5_API bool IsChildVisible(EStateMachineAccessibility Access);
	CRABTOOLSUE5_API bool IsPublic(EStateMachineAccessibility Access);
	CRABTOOLSUE5_API bool IsExtendible(EStateMachineAccessibility Access);
	CRABTOOLSUE5_API bool IsOverrideable(EStateMachineAccessibility Access);
}