#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Binding/DynamicPropertyPath.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "StateMachine/StateMachine.h"
#include "StateMachineBlueprintGeneratedClass.generated.h"



USTRUCT()
struct CRABTOOLSUE5_API FStateArchetypeData
{
	GENERATED_BODY()

	friend class UStateMachineBlueprintGeneratedClass;

private:

	UPROPERTY()
	TObjectPtr<UState> Archetype;

public:

	FStateArchetypeData() {}
	FStateArchetypeData(UState* Machine) : Archetype(Machine) {}

	UPROPERTY()
	bool bIsOverride = false;

	UPROPERTY()
	bool bIsExtension = false;

	UPROPERTY()
	EStateMachineAccessibility Access = EStateMachineAccessibility::PRIVATE;

	const UState* GetArchetype() const { return this->Archetype; }
	UState* GetArchetype() { return this->Archetype; }
	void SetArchetype(UState* NewState) { this->Archetype = NewState; }
};

USTRUCT()
struct CRABTOOLSUE5_API FStateMachineArchetypeData
{
	GENERATED_BODY()

	friend class UStateMachineBlueprintGeneratedClass;

private:

	UPROPERTY()
	TObjectPtr<UStateMachine> Archetype;

public:

	FStateMachineArchetypeData() {}
	FStateMachineArchetypeData(UStateMachine* Machine) : Archetype(Machine) {}

	UPROPERTY()
	TMap<FName, FStateArchetypeData> StateData;

	UPROPERTY()
	bool bInstanceEditable = false;

	UPROPERTY()
	bool bIsVariable = false;

	UPROPERTY()
	bool bCanOverrideStart = false;

	UPROPERTY()
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	void AddStateData(FName StateName, FStateArchetypeData Data);
	const UStateMachine* GetArchetype() const { return this->Archetype; }
	UStateMachine* GetArchetype() { return this->Archetype; }
	void SetArchetype(UStateMachine* NewMachine) { this->Archetype = NewMachine; }
	UStateMachine* CreateStateMachine(UStateMachine* Parent, FName ParentKey) const;
	void CleanAndSanitize();
};

UCLASS()
class CRABTOOLSUE5_API UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FStateMachineArchetypeData Archetype;

	UPROPERTY()
	TMap<FName, FStateMachineArchetypeData> SubArchetypes;

	UPROPERTY()
	TSet<FName> EventSet;

	UPROPERTY();
	TSet<FName> EmittedEvents;

public:

	UState* GetStateData(UStateMachine* Outer, FName Machine, FName StateName);
	//FName GetStartState() const;
	TArray<FString> GetStateOptions(EStateMachineAccessibility Access = EStateMachineAccessibility::PUBLIC) const;
	TArray<FString> GetChildAccessibleSubMachines() const;
	EStateMachineAccessibility GetSubMachineAccessibility(FName Key) const;
	TArray<FName> GetSubMachineOptions() const;
	UStateMachine* ConstructSubMachine(UStateMachine* Outer, FName Key) const;
	FName GetStartState(FName MachineName = NAME_None) const;

	void CollectExtendibleStates(TSet<FString>& StateNames, FName SubMachineName = NAME_None) const;
	void CollectOverrideableStates(TSet<FString>& StateNames, FName SubMachineName = NAME_None) const;

	// Archetype Information functions
	const FStateMachineArchetypeData* GetMachineArchetypeData(FName MachineName = NAME_None) const;
	const FStateArchetypeData* GetStateArchetypeData(FName StateName, FName MachineName = NAME_None) const;
	// End Archetypal information functions

	UStateMachineBlueprintGeneratedClass* GetParent() const
	{
		return Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass());
	}

	bool IsSubMachineNameInUse(FString& Name) const;

	UStateMachine* GetMostRecentParentArchetype(FName SubMachineKey) const;

	// Procedural Construction Functions
	void CleanAndSanitize();
	void AddStateMachine(FStateMachineArchetypeData Data, FName MachineName = NAME_None);
	void VerifyClass(FNodeVerificationContext& Context);
};