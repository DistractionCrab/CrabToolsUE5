#include "StateMachine/AI/AbilityNode.h"
#include "StateMachine/AI/Events.h"
#include "Ability/Ability.h"

UAbilityNode::UAbilityNode()
{
	this->AddEmittedEvent(AI_Events::AI_ABILITY_FINISHED);
}

void UAbilityNode::Initialize_Inner_Implementation()
{
	if (IsValid(this->DefaultAbility))
	{
		this->DefaultAbility->Initialize(this->GetOwner());
	}
}

void UAbilityNode::Enter_Inner_Implementation()
{
	if (IsValid(this->Selected))
	{
		this->Selected->OnAbilityFinished.AddDynamic(this, &UAbilityNode::HandleFinish);

		this->Selected->Start();
	}
	else
	{
		this->EmitEvent(AI_Events::AI_ABILITY_FINISHED);
	}
}

void UAbilityNode::Exit_Inner_Implementation()
{
	if (this->Selected)
	{
		this->Selected->OnAbilityFinished.RemoveAll(this);
		this->Selected = nullptr;
	}
}

void UAbilityNode::EnterWithData_Inner_Implementation(UObject* Data)
{
	if (auto Abi = Cast<UAbility>(Data))
	{
		this->Selected = Abi;
	}
	else
	{
		this->Selected = this->DefaultAbility;		
	}

	this->Enter_Inner();
}

void UAbilityNode::HandleFinish(UAbility* Abi)
{

}