#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "InteractNavLinkProxyFactory.generated.h"

UCLASS(config = Editor)
class CRABTOOLSUE5EDITOR_API UInteractNavLinkProxyFactory : public UActorFactory {
	GENERATED_BODY()

public:
	UInteractNavLinkProxyFactory(const FObjectInitializer& ObjectInitializer);
};