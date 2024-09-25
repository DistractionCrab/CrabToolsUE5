#include "StateMachine/StateMachineInterfaceFactory.h"
#include "StateMachine/StateMachineInterface.h"


#define LOCTEXT_NAMESPACE "UStateMachineInterfaceFactory"

UStateMachineInterfaceFactory::UStateMachineInterfaceFactory() {
	SupportedClass = UStateMachineInterface::StaticClass();
	bCreateNew = true;
}

UObject* UStateMachineInterfaceFactory::FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn, 
		FName CallingContext) 
{
	return NewObject<UStateMachineInterface>(InParent, Class, Name, Flags, Context);
}


UObject* UStateMachineInterfaceFactory::FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn)
{
	return this->FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}


bool UStateMachineInterfaceFactory::ShouldShowInNewMenu() const {
	return true;
}

#undef LOCTEXT_NAMESPACE