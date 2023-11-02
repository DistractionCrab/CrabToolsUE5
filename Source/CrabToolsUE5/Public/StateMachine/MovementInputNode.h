#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/ProcStateMachine.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovementInputNode.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UMovementInputNode : public UStateNode
{
	GENERATED_BODY()

	// Possible perspective of the owning pawn.
	TWeakObjectPtr<UPerspectiveManager> Perspective;
	// Saved reference to the owner if it is a Pawn.
	TWeakObjectPtr<APawn>PawnOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	bool Active = false;

public:

	virtual void Initialize_Implementation(UProcStateMachine* POwner) override;
	virtual void Enter_Implementation() override;
	virtual void Exit_Implementation() override;
	
	void MoveCallback(const FInputActionValue& Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void ApplyMovement(FVector2D InputAxis);
	virtual void ApplyMovement_Implementation(FVector2D InputAxis);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	UPerspectiveManager* GetPerspective() { return this->Perspective.Get(); }
};