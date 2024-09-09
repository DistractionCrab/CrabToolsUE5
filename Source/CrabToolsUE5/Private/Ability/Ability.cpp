#include "Ability/Ability.h"

void UAbility::Start()
{
	this->Start_Inner();
	this->OnAbilityStarted.Broadcast(this);
}

void UAbility::Perform()
{

}

void UAbility::Tick(float DeltaTime)
{

}

void UAbility::Finish()
{

}