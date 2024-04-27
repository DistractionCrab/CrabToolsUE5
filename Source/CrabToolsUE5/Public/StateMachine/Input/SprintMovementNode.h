#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/Input/MovementInputNode.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SprintMovementNode.generated.h"

DECLARE_DELEGATE_OneParam(FSprintInputCallback, bool);
DECLARE_DELEGATE_OneParam(FWalkInputCallback, bool);

UCLASS(Blueprintable, EditInlineNew)
class CRABTOOLSUE5_API USprintMovementNode : public UMovementInputNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* WalkAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	float SprintRatio = 1.0;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	float JogRatio = 0.5;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	float WalkRatio = 0.25;

	bool bForceWalk = false;
	bool bForceSprint = false;

public:

	virtual void Initialize_Implementation(UStateMachine* POwner) override;
	virtual void ApplyMovement_Implementation(FVector2D InputAxis) override;

	void ApplySprint(bool DoSprint) { this-> bForceSprint = DoSprint; }
	void ApplyWalk(bool DoWalk) { this-> bForceWalk = DoWalk; }
};