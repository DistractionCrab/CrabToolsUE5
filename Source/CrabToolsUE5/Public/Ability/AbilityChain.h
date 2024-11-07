#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Ability/Ability.h"
#include "AbilityChain.generated.h"

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UAbilityChain : public UAbility
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Instanced, EditDefaultsOnly, Category="Ability",
		meta=(AllowPrivateAccess))
	TArray<TObjectPtr<UAbility>> AbilityChain;

	int ActiveIndex = 0;

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Start_Inner_Implementation() override;
	virtual void Perform_Inner_Implementation() override;
	virtual void Tick_Inner_Implementation(float DeltaTime) override;
	virtual void Finish_Inner_Implementation() override;
	virtual bool RequiresTick_Implementation() const override;

private:

	UFUNCTION()
	void HandleFinish(UAbility* Abi);
};
