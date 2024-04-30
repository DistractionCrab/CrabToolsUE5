#include "Factory/InteractNavLinkProxyFactory.h"
#include "AI/InteractNavlinkProxy.h"
#include "IPlacementModeModule.h"

#define LOCTEXT_NAMESPACE "UInteractNavLinkProxyFactory"

UInteractNavLinkProxyFactory::UInteractNavLinkProxyFactory(const FObjectInitializer& ObjectInitializer)
{
	DisplayName = LOCTEXT("InteractNavLinkProxy", "Interactable Nav Link");
	NewActorClass = AInteractNavLinkProxy::StaticClass();
}

#undef LOCTEXT_NAMESPACE