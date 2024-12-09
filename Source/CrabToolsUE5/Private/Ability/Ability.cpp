#include "Ability/Ability.h"

DEFINE_LOG_CATEGORY(LogAbility);

void UAbility::Initialize(AActor* POwner)
{
	if (!this->Owner)
	{
		this->Owner = POwner;
	}
	this->Initialize_Inner();
}

void UAbility::Start()
{
	if (!this->bActive && this->bUseable)
	{
		this->bActive = true;
		this->Start_Inner();
		this->OnAbilityStarted.Broadcast(this);		
	}
}

UAbility* UAbility::GetParent() const
{
	return Cast<UAbility>(this->GetOuter());
}

void UAbility::Perform()
{
	if (this->bActive)
	{
		this->Perform_Inner();
		this->OnAbilityPerformed.Broadcast(this);
	}
}

void UAbility::SetUseable(bool bNewUseable)
{
	this->bUseable = bNewUseable;
	this->OnAbilityUseabilityChanged.Broadcast(this, bNewUseable);
}

void UAbility::Delete()
{
	this->Delete_Inner();
	this->OnAbilityDeleted.Broadcast(this);
}

void UAbility::Tick(float DeltaTime)
{
	if (this->bActive)
	{
		this->Tick_Inner(DeltaTime);
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

void UAbility::Cancel()
{
	this->Cancel_Inner();
	this->OnAbilityCanceled.Broadcast(this);
}

UWorld* UAbility::GetWorld() const
{
	//Return null if the called from the CDO, or if the outer is being destroyed
	if (!HasAnyFlags(RF_ClassDefaultObject) && !GetOuter()->HasAnyFlags(RF_BeginDestroyed) && !GetOuter()->IsUnreachable())
	{
		//Try to get the world from the owning actor if we have one
		if (this->Owner)
		{
			return this->Owner->GetWorld();
		}
	}

	//Else return null - the latent action will fail to initialize
	return nullptr;
}