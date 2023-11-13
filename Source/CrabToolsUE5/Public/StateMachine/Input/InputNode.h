#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/ProcStateMachine.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputNode.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UInputNode : public UStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProcStateMachine|Input", meta=(AllowPrivateAccess = "true"))
	class UInputAction* Action;

	// Saved reference to the owner if it is a Pawn.
	TWeakObjectPtr<APawn> PawnOwner;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProcStateMachine|Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bTrigger = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProcStateMachine|Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProcStateMachine|Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bOngoing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProcStateMachine|Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bCompleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProcStateMachine|Input|Events", meta=(AllowPrivateAccess = "true"))
	bool bCanceled = false;

	

public:

	virtual void Initialize_Implementation(UProcStateMachine* POwner) override;
	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void TriggerCallback(const FInputActionValue& Value);
	virtual void TriggerCallback_Implementation(const FInputActionValue& Value);
	void TriggerCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void StartCallback(const FInputActionValue& Value);
	virtual void StartCallback_Implementation(const FInputActionValue& Value);
	void StartCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void OngoingCallback(const FInputActionValue& Value);
	virtual void OngoingCallback_Implementation(const FInputActionValue& Value);
	void OngoingCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void CompletedCallback(const FInputActionValue& Value);
	virtual void CompletedCallback_Implementation(const FInputActionValue& Value);
	void CompletedCallback_Internal(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void CanceledCallback(const FInputActionValue& Value);
	virtual void CanceledCallback_Implementation(const FInputActionValue& Value);
	void CanceledCallback_Internal(const FInputActionValue& Value);

	

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	APawn* GetPawn() { return this->PawnOwner.Get();  }
};