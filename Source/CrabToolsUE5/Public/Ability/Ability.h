#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Ability.generated.h"

/**
 * Class for player abilities. Abilities consist of a start, a continuous action,
 * a discrete action that happens on request, and an finish.
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UAbility : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Transient, DuplicateTransient,
		meta=(ExposeOnSpawn=true, AllowPrivateAccess))
	TObjectPtr<AActor> Owner;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Ability",
		meta=(AllowPrivateAccess))
	bool bNeedsTick = false;

	bool bActive = false;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityStarted, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityStarted OnAbilityStarted;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityPerformed, UAbility*, Ability);
	/* Called when the ability performs its body. Either through Perform() */
	UPROPERTY(BlueprintAssignable, Category="Ability")
	FAbilityStarted OnAbilityPerformed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityFinished, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityStarted OnAbilityFinished;

public:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Initialize(AActor* POwner);

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Start();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Perform();

	UFUNCTION(BlueprintCallable, Category="Ability")
	void Tick(float DeltaTime);

	/* Call this to finish the ability if you are the controller. */
	UFUNCTION(BlueprintCallable, Category="Ability")
	void Finish();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsActive() const { return this->bActive; }

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AActor* GetOwner() const { return this->Owner; }



protected:

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void Start_Inner();
	virtual void Start_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void Perform_Inner();
	virtual void Perform_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void Tick_Inner(float DeltaTime);
	virtual void Tick_Inner_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	void Finish_Inner();
	virtual void Finish_Inner_Implementation() {}
};
