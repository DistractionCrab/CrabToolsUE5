#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "TransientTask.generated.h"

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Abstract, Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UTransientTask : public UObject
{
	GENERATED_BODY()
	
public:

	DECLARE_MULTICAST_DELEGATE(FTaskCompleted);
	FTaskCompleted OnTaskCompleted;

public:

	/* Starts the task at hand. */
	UFUNCTION(BlueprintNativeEvent, Category="Tasks")
	void BeginTask();
	void BeginTask_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Tasks")
	void TickTask(float DeltaTime);
	void TickTask_Implementation(float DeltaTime) {}

	/* Call to manually end the task. */
	UFUNCTION(BlueprintNativeEvent, Category = "Tasks")
	void EndTask();
	void EndTask_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "Tasks")
	bool RequiresTick();
	bool RequiresTick_Implementation() { return false; }

	UFUNCTION(BlueprintCallable, Category="Tasks")
	void TaskCompleted();
};
