#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Ability/Ability.h"
#include "AnimAbility.generated.h"

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UAnimAbility : public UAbility
{
	GENERATED_BODY()

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditDefaultsOnly, Category = "Animation", meta = (AllowPrivateAccess))
		TSubclassOf<AActor> ActorClass;
	#endif //WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category = "Animation",
		meta = (AllowPrivateAccess, GetOptions = "GetComponentOptions"))
	FName ComponentName;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> Component;

public:

	UAnimAbility();

protected:

	void Initialize_Inner_Implementation() override;
	void Start_Inner_Implementation() override;
	
private:

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetComponentOptions() const;
	#endif //WITH_EDITOR

	UFUNCTION()
	void AnimNotify_AbilityFinish();

	UFUNCTION()
	void AnimNotify_AbilityPerform();

};
