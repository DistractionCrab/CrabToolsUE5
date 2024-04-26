#pragma once
 
#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "StateMachine/ProcStateMachine.h"
#include "ProcStateMachineBlueprintFactory.generated.h"
 
UCLASS(HideCategories=Object, MinimalAPI)
class UProcStateMachineBlueprintFactory : public UFactory
{
	GENERATED_BODY()

private:
	
	// The type of blueprint that will be created
	UPROPERTY(EditAnywhere, Category="PSMBlueprintFactory")
	TEnumAsByte<enum EBlueprintType> BlueprintType;

	// The parent class of the created blueprint
	UPROPERTY(EditAnywhere, Category="PSMBlueprintFactory", meta=(AllowAbstract = ""))
	TSubclassOf<class UProcStateMachine> ParentClass;

	UPROPERTY(Transient)
	TObjectPtr<UClass> RootMachineClass;

public:
	UProcStateMachineBlueprintFactory();

	virtual bool ConfigureProperties() override;

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