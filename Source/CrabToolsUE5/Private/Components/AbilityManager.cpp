#include "Components/AbilityManager.h"

void UAbilityManagerComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAbilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}