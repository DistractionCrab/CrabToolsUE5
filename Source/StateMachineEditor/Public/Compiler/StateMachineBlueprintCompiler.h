#pragma once

#include "CoreMinimal.h"
#include "EngineLogs.h"
#include "Engine/Blueprint.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "KismetCompiler.h"
#include "KismetCompilerModule.h"
#include "ComponentReregisterContext.h"


class FProperty;
class UEdGraph;
class UStateMachineComponent;
class UStateMachineBlueprintGeneratedClass;
//class UWidget;
//class UWidgetAnimation;
//class UWidgetGraphSchema;

//////////////////////////////////////////////////////////////////////////
// FStateMachineBlueprintCompiler 

class STATEMACHINEEDITOR_API FStateMachineBlueprintCompiler : public IBlueprintCompiler
{

public:
	FStateMachineBlueprintCompiler();

	bool CanCompile(const UBlueprint* Blueprint) override;
	void PreCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	void Compile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions, FCompilerResultsLog& Results) override;
	void PostCompile(UBlueprint* Blueprint, const FKismetCompilerOptions& CompileOptions) override;
	bool GetBlueprintTypesForClass(UClass* ParentClass, UClass*& OutBlueprintClass, UClass*& OutBlueprintGeneratedClass) const override;

private:

	/** The temporary variable that captures and reinstances components after compiling finishes. */
	TComponentReregisterContext<UStateMachineComponent>* ReRegister;

	/**
	* The current count on the number of compiles that have occurred.  We don't want to re-register components until all
	* compiling has stopped.
	*/
	int32 CompileCount;

};

//////////////////////////////////////////////////////////////////////////
// FStateMachineBlueprintCompilerContext


class STATEMACHINEEDITOR_API FStateMachineBlueprintCompilerContext : public FKismetCompilerContext
{
protected:
	typedef FKismetCompilerContext Super;

public:
	FStateMachineBlueprintCompilerContext(UStateMachineBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions);
	virtual ~FStateMachineBlueprintCompilerContext();

protected:
	void ValidateStateMachineNames();

	//~ Begin FKismetCompilerContext
	//virtual UEdGraphSchema_K2* CreateSchema() override;
	virtual void CreateFunctionList() override;
	virtual void SpawnNewClass(const FString& NewClassName) override;
	virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
	//virtual void PrecompileFunction(FKismetFunctionContext& Context, EInternalCompilerFlags InternalFlags) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOutOldCDO) override;
	virtual void SaveSubObjectsFromCleanAndSanitizeClass(FSubobjectCollection& SubObjectsToSave, UBlueprintGeneratedClass* ClassToClean) override;
	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	virtual void CreateClassVariablesFromBlueprint() override;
	virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject);
	virtual void FinishCompilingClass(UClass* Class) override;
	virtual bool ValidateGeneratedClass(UBlueprintGeneratedClass* Class) override;
	virtual void OnPostCDOCompiled(const UObject::FPostCDOCompiledContext& Context) override;
	//~ End FKismetCompilerContext

	//void VerifyEventReplysAreNotEmpty(FKismetFunctionContext& Context);
	//void VerifyFieldNotifyFunction(FKismetFunctionContext& Context);

public:
	UStateMachineBlueprint* StateMachineBlueprint() const { return Cast<UStateMachineBlueprint>(Blueprint); }

	//void AddExtension(UStateMachineBlueprintGeneratedClass* Class, UStateMachineBlueprintGeneratedClassExtension* Extension);

	struct STATEMACHINEEDITOR_API FCreateVariableContext
	{
	public:
		FProperty* CreateVariable(const FName Name, const FEdGraphPinType& Type) const;
		void AddGeneratedFunctionGraph(UEdGraph* Graph) const;
		UStateMachineBlueprint* GetStateMachineBlueprint() const;

		//UE_DEPRECATED(5.4, "GetSkeletonGeneratedClass renamed to GetGeneratedClass")
		//UStateMachineBlueprintGeneratedClass* GetSkeletonGeneratedClass() const;
		UStateMachineBlueprintGeneratedClass* GetGeneratedClass() const;
		EKismetCompileType::Type GetCompileType() const;

	private:
		friend FStateMachineBlueprintCompilerContext;
		FCreateVariableContext(FStateMachineBlueprintCompilerContext& InContext);
		FStateMachineBlueprintCompilerContext& Context;
	};

	struct STATEMACHINEEDITOR_API FCreateFunctionContext
	{
	public:
		void AddGeneratedFunctionGraph(UEdGraph*) const;
		UStateMachineBlueprintGeneratedClass* GetGeneratedClass() const;

	private:
		friend FStateMachineBlueprintCompilerContext;
		FCreateFunctionContext(FStateMachineBlueprintCompilerContext& InContext);
		FStateMachineBlueprintCompilerContext& Context;
	};


protected:

	UStateMachineBlueprintGeneratedClass* NewStateMachineBlueprintClass;
	//UWidgetGraphSchema* WidgetSchema;

	/*
	// Map of properties created for widgets; to aid in debug data generation
	TMap<UWidget*, FProperty*> WidgetToMemberVariableMap;

	// Map of properties created in parent widget for bind widget validation
	TMap<UWidget*, FProperty*> ParentWidgetToBindWidgetMap;

	// Map of properties created for widget animations; to aid in debug data generation
	TMap<UWidgetAnimation*, FProperty*> WidgetAnimToMemberVariableMap;
	*/
};