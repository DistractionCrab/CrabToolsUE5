#include "StateMachine/StateMachineBlueprintFactory.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"
#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"


#define LOCTEXT_NAMESPACE "UStateMachineBlueprintFactory"


class FStateMachineClassFilter : public IClassViewerFilter
{
public:
	/** All children of these classes will be included unless filtered out by another setting. */
	TSet <const UClass*> AllowedChildrenOfClasses;

	/** Disallowed class flags. */
	EClassFlags DisallowedClassFlags;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
	}
	
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}

};

UStateMachineBlueprintFactory::UStateMachineBlueprintFactory() {
	SupportedClass = UStateMachineBlueprint::StaticClass();
	bCreateNew = true;
}

UObject* UStateMachineBlueprintFactory::FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn, 
		FName CallingContext) 
{
	check(Class->IsChildOf(UStateMachineBlueprint::StaticClass()));

	UClass* CurrentParentClass = this->ParentClass;
	if (CurrentParentClass == nullptr) {
		CurrentParentClass = UStateMachine::StaticClass();
	}

	bool c1 = CurrentParentClass == nullptr;
	bool c2 = !FKismetEditorUtilities::CanCreateBlueprintOfClass(CurrentParentClass);
	bool c3 = !CurrentParentClass->IsChildOf(UStateMachine::StaticClass());

	if (c1 || c2 || c3)	{
		FFormatNamedArguments Args;
		Args.Add(
			TEXT("ClassName"), 
			CurrentParentClass ? FText::FromString(CurrentParentClass->GetName() ) : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(
			EAppMsgType::Ok, 
			FText::Format(
				LOCTEXT("CannotCreatePSMBlueprint", 
					"Cannot create a Machine Blueprint based on the class '{ClassName}'."), 
				Args ));
		return nullptr;
	} 
	else {
		auto NewBP = CastChecked<UStateMachineBlueprint>(
			FKismetEditorUtilities::CreateBlueprint(
				CurrentParentClass, 
				InParent, 
				Name, 
				BlueprintType, 
				UStateMachineBlueprint::StaticClass(), 
				UStateMachineBlueprintGeneratedClass::StaticClass(), 
				CallingContext));

		return NewBP;
	}	
}


UObject* UStateMachineBlueprintFactory::FactoryCreateNew(
		UClass* Class, 
		UObject* InParent, 
		FName Name, 
		EObjectFlags Flags, 
		UObject* Context, 
		FFeedbackContext* Warn)
{
	return this->FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}


bool UStateMachineBlueprintFactory::ConfigureProperties() {
	FClassViewerModule& ClassViewerModule = 
		FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	FClassViewerInitializationOptions Options;
	Options.DisplayMode = EClassViewerDisplayMode::Type::TreeView;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.bShowNoneOption = false;
	Options.bExpandAllNodes = true;
	//Options.bShowDefaultClasses = GetDefault<UUMGEditorProjectSettings>()->bUseUserWidgetParentDefaultClassViewerSelector;
	//Options.bShowClassesViewer = GetDefault<UUMGEditorProjectSettings>()->bUseUserWidgetParentClassViewerSelector;

	TSharedPtr<FStateMachineClassFilter> Filter = MakeShareable(new FStateMachineClassFilter);
	Options.ClassFilters.Add(Filter.ToSharedRef());

	if (Options.ExtraPickerCommonClasses.Num() == 0) {
		Options.ExtraPickerCommonClasses.Add(UStateMachine::StaticClass());
	}

	Filter->DisallowedClassFlags = CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Hidden | CLASS_HideDropDown;
	Filter->AllowedChildrenOfClasses.Add(UStateMachine::StaticClass());

	const FText TitleText = LOCTEXT("CreateWidgetBlueprint", "Pick Parent Class for New Widget Blueprint");

	UClass* ChosenParentClass = nullptr;
	bool isSuccessful = SClassPickerDialog::PickClass(TitleText, Options, ChosenParentClass, UStateMachine::StaticClass());
	ParentClass = ChosenParentClass ? ChosenParentClass : UStateMachine::StaticClass();

	return isSuccessful;

}

bool UStateMachineBlueprintFactory::ShouldShowInNewMenu() const {
	return true;
}

#undef LOCTEXT_NAMESPACE