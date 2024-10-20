#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "TargetingController.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UTargetingControllerInterface : public UInterface
{
    GENERATED_BODY()
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConfirmTargetsMulti);
DECLARE_DYNAMIC_DELEGATE(FConfirmTargetsSingle);

class ITargetingControllerInterface
{
    GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	AActor* GetUsingActor() const;
	virtual AActor* GetUsingActor_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void Initialize();
	virtual void Initialize_Implementation() {}

	/* Manually add a target to this controller. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddTarget(AActor* Target);
	virtual void AddTarget_Implementation(AActor* Target) {}

	/* Wehther or not the targeting controller supports direct adding of targets. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	bool SupportsDirectAdd() const;
	virtual bool SupportsDirectAdd_Implementation() const { return false; }

	/* Save the currently selected targets */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void PushTarget();
	virtual void PushTarget_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void PopTarget();
	virtual void PopTarget_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void RemoveTarget(int Index);
	virtual void RemoveTarget_Implementation(int Index) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void Confirm();
	virtual void Confirm_Implementation() {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void AddListener(const FConfirmTargetsSingle& Callback);
	virtual void AddListener_Implementation(const FConfirmTargetsSingle& Callback) {}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void GetTargets(TArray<AActor*>& Actors) const;
	virtual void GetTargets_Implementation(TArray<AActor*>& Actors) const {};

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting")
	void GetTargetPoints(TArray<FVector>& Points) const;
	virtual void GetTargetPoints_Implementation(TArray<FVector>& Points) const {};
};