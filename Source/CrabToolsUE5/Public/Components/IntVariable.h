// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <limits>
#include "Operators/VariableOperator.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/Delegate.h"
#include "IntVariable.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FIntObserverCollection, int, Old, int, New, UIntVariable*, Variable);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FIntObserver, int, Old, int, New, UIntVariable*, Variable);
DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(int, FIntModifier, int, New, int, Old, UIntVariable*, Variable);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FIntOperatorObserver, int, Old, UVariableOperator*, Opp, UIntVariable*, Variable);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UIntVariable : public UActorComponent
{
	GENERATED_BODY()



	TArray<FIntOperatorObserver> ChangeOperators;
	TArray<FIntModifier> ChangeModifiers;
	FIntObserverCollection ChangeObserversCollection;

	TArray<FIntModifier> MaxChangeModifiers;
	FIntObserverCollection MaxChangeObserversCollection;

	TArray<FIntModifier> MinChangeModifiers;
	FIntObserverCollection MinChangeObserversCollection;

protected:
	UPROPERTY(EditAnywhere, BlueprintGetter = GetValue, Category = "Variables")
	int Value;

	UPROPERTY(EditAnywhere, Category = "Variables")
	int Min = MIN_int32;

	UPROPERTY(EditAnywhere, Category = "Variables")
	int Max = MAX_int32;

	UPROPERTY(EditAnywhere, Category = "Variables")
	int TickAmount = 0;

public:	
	// Sets default values for this component's properties
	UIntVariable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure, Category = "Variables")
	int GetValue() { return this->Value; }

	/* Set the value of the new variable, applying and ModifierListeners and clamping each modifier. */
	UFUNCTION(BlueprintCallable, Category = "Variables")
	int SetValue(int NewValue);

	UFUNCTION(BlueprintCallable, Category = "Variables")
	int OperateValue(UVariableOperator* Op);

	UFUNCTION(BlueprintPure, Category = "Variables")
	int GetMax() { return this->Max; }

	UFUNCTION(BlueprintPure, Category = "Variables")
	int GetMin() { return this->Min; }

	UFUNCTION(BlueprintCallable, Category = "Variables")
	void ListenForChange(const FIntObserver& obs) {
		this->ChangeObserversCollection.Add(obs);
	}

	UFUNCTION(BlueprintCallable, Category = "Variables")
	void ListenForModification(const FIntModifier& obs) {
		this->ChangeModifiers.Add(obs);
	}

	UFUNCTION(BlueprintCallable, Category = "Variables")
	void ListenForOperator(const FIntOperatorObserver& obs) {
		this->ChangeOperators.Add(obs);
	}

		
};
