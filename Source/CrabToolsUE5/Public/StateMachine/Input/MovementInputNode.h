#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/Input/InputNode.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovementInputNode.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UMovementInputNode : public UInputNode
{
	GENERATED_BODY()

	// Possible perspective of the owning pawn.
	TWeakObjectPtr<UPerspectiveManager> Perspective;
	


public:

	UMovementInputNode();

	virtual void Initialize_Implementation(UProcStateMachine* POwner) override;
	virtual void TriggerCallback_Implementation(const FInputActionValue& Value);
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void ApplyMovement(FVector2D InputAxis);
	virtual void ApplyMovement_Implementation(FVector2D InputAxis);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	UPerspectiveManager* GetPerspective() { return this->Perspective.Get(); }
};