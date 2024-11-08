#pragma once

#include "CoreMinimal.h"
#include "Ability/Ability.h"
#include "AnimMontageAbility.generated.h"

/**
 * Ability which plays a montage, and performs actions when montage events happen.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UAnimMontageAbility : public UAbility
{
	GENERATED_BODY()

	#if WITH_EDITORONLY_DATA
		/* Only used in editor for discovering components. */
		UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess))
		TSubclassOf<AActor> ActorClass;
	#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess, GetOptions = "GetComponentOptions"))
	FName ComponentName;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> Component;

	UPROPERTY(EditDefaultsOnly, Category="Animation")
	TObjectPtr<UAnimMontage> Montage;

public:

	UAnimMontageAbility();

protected:

	virtual void Initialize_Inner_Implementation() override;
	virtual void Start_Inner_Implementation() override;
	virtual void Finish_Inner_Implementation() override;
	
private:
	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetComponentOptions() const;
	#endif //WITH_EDITOR

	UFUNCTION()
	void AnimNotify_AbilityPerform();
};
