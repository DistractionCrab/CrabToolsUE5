#include "Ability/Ability.h"

void UAbility::Initialize(AActor* POwner)
{
	this->Owner = POwner;
	this->Initialize_Inner();
}

void UAbility::Start()
{
	if (!this->bActive)
	{
		this->bActive = true;
		this->Start_Inner();
		this->OnAbilityStarted.Broadcast(this);		
	}
}

void UAbility::Perform()
{
	if (this->bActive)
	{
		this->Perform_Inner();
		this->OnAbilityPerformed.Broadcast(this);
	}
}

void UAbility::Tick(float DeltaTime)
{
	if (this->bActive)
	{
		this->Tick_Inner(DeltaTime);
		this->OnAbilityPerformed.Broadcast(this);
	}
}

void UAbility::Finish()
{
	if (this->bActive)
	{
		this->bActive = false;
		this->Finish_Inner();
		this->OnAbilityFinished.Broadcast(this);
	}
}

