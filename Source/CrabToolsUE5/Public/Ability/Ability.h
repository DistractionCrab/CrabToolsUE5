#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Actors/Targeting/BaseTargetingActor.h"
#include "Ability.generated.h"

/* This interface is used for containers of abilitys, e.g. Targeting Controllers. */
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UHasAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

class IHasAbilityInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Ability")
	UAbility* GetAbility();
	UAbility* GetAbility_Implementation() { return nullptr; }
};

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

	bool bActive = false;

	UPROPERTY(Transient)
	TObjectPtr<ABaseTargetingActor> Targeting;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityStarted, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityStarted OnAbilityStarted;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityPerformed, UAbility*, Ability);

	/* Called when the ability performs its body. */
	UPROPERTY(BlueprintAssignable, Category="Ability")
	FAbilityStarted OnAbilityPerformed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityFinished, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityStarted OnAbilityFinished;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityCanceled, UAbility*, Ability);
	UPROPERTY(BlueprintAssignable, Category = "Ability")
	FAbilityCanceled OnAbilityCanceled;

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

	/* Called when the ability was canceled. Can be used even when not active. */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void Cancel();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsActive() const { return this->bActive; }

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const { return false; }

	UFUNCTION(BlueprintCallable, Category = "Ability")
	AActor* GetOwner() const { return this->Owner; }

	virtual UWorld* GetWorld() const override;

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Cancel_Inner();
	void Cancel_Inner_Implementation() {}

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Start_Inner();
	virtual void Start_Inner_Implementation() {}

	/* Start the ability; Used for initializing data. */
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	bool RequiresTick_Inner() const;
	virtual bool RequiresTick_Inner_Implementation() const { return false; }

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Perform_Inner();
	virtual void Perform_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Tick_Inner(float DeltaTime);
	virtual void Tick_Inner_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
	void Finish_Inner();
	virtual void Finish_Inner_Implementation() {}

};
