#pragma once

#include "CoreMinimal.h"

namespace StateMachineColors
{
	namespace NodeBody
	{
		const FLinearColor Default(0.1f, 0.1f, 0.1f);
		const FLinearColor Root(0.5f, 0.5f, 0.5f, 0.1f);
		const FLinearColor Error(1.0f, 0.0f, 0.0f);
	}

	namespace NodeBorder
	{
		/* Color Used for start state nodes. */
		const FLinearColor Start(0.8f, 0.08f, 0.08f);

		/* Color Used for node extensions & are protected nodes. */
		const FLinearColor ExtensionProtected(0.0f, 0.45f, 0.9f);

		/* Color Used for node extensions & are overrideable nodes. */
		const FLinearColor ExtensionOverride(0.0f, 0.5f, 0.1f);

		/* Used if some internal error has occurred to the node. */
		const FLinearColor InternalError(1.0f, 0.0f, 0.0f);

		const FLinearColor Root(0.2f, 0.2f, 0.2f, 0.8f);

		/* Color Used for alias state nodes. */
		const FLinearColor Alias(0, 0.73f, 0.27f);
	}

	namespace Pin
	{
		const FLinearColor Default(0.02f, 0.02f, 0.02f);
	}
}