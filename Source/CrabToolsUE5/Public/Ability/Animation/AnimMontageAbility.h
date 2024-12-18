#pragma once

#include "CoreMinimal.h"
#include "Ability/Animation/AnimAbility.h"
#include "AnimMontageAbility.generated.h"

/**
 * Ability which plays a montage, and performs actions when montage events happen.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UAnimMontageAbility : public UAnimAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> Montage;

public:

protected:

	virtual void Start_Inner_Implementation() override;
	
};
