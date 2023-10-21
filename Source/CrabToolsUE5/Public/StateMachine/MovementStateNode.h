#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateMachine/ProcStateMachine.h"
#include "Components/PerspectiveManager.h"
#include "GameFramework/Pawn.h"
#include "MovementStateNode.generated.h"

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UMovementStateNode : public UStateNode
{
	GENERATED_BODY()

	// Possible perspective of the owning pawn.
	UPerspectiveManager* Perspective;
	// Saved reference to the owner if it is a Pawn.
	APawn* PawnOwner;

	FVector2D InputAxis;

public:
	UMovementStateNode();

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	float Speed;

	virtual void Initialize_Implementation(UProcStateMachine* POwner) override;


	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void UpdateInputAxis(FVector2D FVec);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void ApplyMovement();
	virtual void ApplyMovement_Implementation();
};