#pragma once
 
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "StateMachine/StateMachine.h"
#include "StateMachineInterfaceFactory.generated.h"
 
UCLASS(HideCategories=Object, MinimalAPI)
class UStateMachineInterfaceFactory : public UFactory
{
	GENERATED_BODY()

public:

	UStateMachineInterfaceFactory();

	virtual bool ShouldShowInNewMenu() const override;

	virtual UObject* FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn, 
		FName CallingContext) override;

	virtual UObject* FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn) override;
};