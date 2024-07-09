#include "StateMachine/StateMachine.h"

#include "Logging/StructuredLog.h"
#include "Algo/Reverse.h"
#include "EdGraphSchema_K2.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/ArrayNode.h"

namespace Constants
{

}

#pragma region StateMachine Code

void UStateMachine::Initialize_Internal(AActor* POwner)
{
	this->Owner = POwner;
	this->InitFromArchetype();
	this->Initialize();

	/*
	for (auto& Pair : this->SharedNodes) {
		this->Substitute(Pair.Key, Pair.Value);
	}
	
	for (auto& pair : this->Graph) {
		auto& StateName = pair.Key;
		auto& StateData = pair.Value;

		if (StateData.Node) {
			StateData.Node->Initialize_Internal(this);
		}
	}
	*/

	// Shared nodes always exist, and should be initialize from the beginning.
	for (auto& Node : this->SharedNodes)
	{
		Node.Value->Initialize_Internal(this);
	}

	this->RebindConditions();
	this->UpdateState(this->StartState);	
}

void UStateMachine::Initialize_Implementation() {}

UStateMachine* UStateNode::GetMachineAs(TSubclassOf<UStateMachine> SClass, ESearchResult& Result) {
	auto Class = SClass.Get();
	auto Machine = this->GetMachine();
	if (Class && Machine) {
		if (Machine->IsA(Class)) {
			Result = ESearchResult::Found;
			return Machine;
		}
	}

	Result = ESearchResult::Found;
	return nullptr;
}

AActor* UStateMachine::GetOwner() {
	return this->Owner;
}

void UStateMachine::AddState(FName StateName)
{
	this->Graph.Add(StateName, FStateData());
}

void UStateMachine::UpdateState(FName Name) {
	UE_LOG(LogTemp, Warning, TEXT("Updating State"));
	if (Name != this->CurrentStateName) {
		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		auto TID = this->TRANSITION.EnterTransition();
		// Always exit on the node, regardless of further state transitions.
		// If there were any updates to the state prior to this, then Exit will have
		// called already, and this function will have returned;
		if (CurrentState && CurrentState->Node) CurrentState->Node->Exit_Internal();

		if (this->TRANSITION.Valid(TID)) {
			UE_LOG(LogTemp, Warning, TEXT("Attemping to transition to the new state."));
			this->CurrentStateName = Name;
			CurrentState = this->GetCurrentState();
			if (CurrentState && CurrentState->Node) CurrentState->Node->Enter_Internal();

			
			if (this->TRANSITION.Valid(TID)) {
				// Alert all listeners, and if one of them changes the state, return.
				for (auto& Listener : this->StateChangeEvents) {
					Listener.ExecuteIfBound(OldState, Name);
					if (!this->TRANSITION.Valid(TID)) {
						return;
					}
				}
			}
		}	
	}
}

void UStateMachine::UpdateStateWithData(FName Name, UObject* Data) {
	if (this->Graph.Contains(Name) && Name != this->CurrentStateName) {
		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		auto TID = this->TRANSITION.EnterTransition();
		// Always exit on the node, regardless of further state transitions.
		// If there were any updates to the state prior to this, then Exit will have
		// called already, and this function will have returned;
		if (CurrentState && CurrentState->Node) CurrentState->Node->ExitWithData_Internal(Data);

		if (this->TRANSITION.Valid(TID)) {
			this->CurrentStateName = Name;
			CurrentState = this->GetCurrentState();
			if (CurrentState->Node) CurrentState->Node->EnterWithData_Internal(Data);


			if (this->TRANSITION.Valid(TID)) {
				// Alert all listeners, and if one of them changes the state, return.
				for (auto& Listener : this->StateChangeEvents) {
					Listener.ExecuteIfBound(OldState, Name);
					if (!this->TRANSITION.Valid(TID)) {
						return;
					}
				}
			}
		}
	}
}

void UStateMachine::Tick(float DeltaTime) {
	if (this->CurrentStateName != NAME_None) {
		auto State = this->GetCurrentState();
		if (State && State->Node) {
			State->Node->Tick(DeltaTime);	
		}		
	}
}


void UStateMachine::Reset() {
	this->UpdateState(this->StartState);
}

void UStateMachine::Event(FName EName) {
	// Need to validate possible transitions.
	auto TID = this->TRANSITION.CurrentID();
	auto CurrentState = this->GetCurrentState();

	if (CurrentState) {
		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->Transitions.Contains(EName)) {
			auto TData = CurrentState->Transitions[EName];
			if (TData.ConditionCallback.IsBound()) {
				if (TData.ConditionCallback.Execute()) {
					this->UpdateState(TData.Destination);
				}
			}
			else {
				UE_LOGFMT(LogTemp, Error, "Condition Delegate Unbound? Condition Name: {0}", TData.Condition);
			}
		}
		

		// If the current node's event code didn't change the graph's state, then we check for
		// static event transitions.
		if (this->TRANSITION.Valid(TID)) {
			if (CurrentState->Node) CurrentState->Node->Event_Internal(EName);	
		}
	}	
}


void UStateMachine::EventWithData(FName EName, UObject* Data) {
	// Need to validate possible transitions.
	auto TID = this->TRANSITION.CurrentID();
	auto CurrentState = this->GetCurrentState();

	if (CurrentState) {
		// First we check if there are any declarative events to handle for this state.
		if (CurrentState->Transitions.Contains(EName)) {
			auto TData = CurrentState->Transitions[EName];
			if (TData.DataConditionCallback.IsBound()) {
				if (TData.DataConditionCallback.Execute(Data)) {
					this->UpdateStateWithData(TData.Destination, Data);
				}
			}
			else {
				UE_LOGFMT(LogTemp, Error, "Condition Delegate Unbound? Condition Name: {0}", TData.Condition);
			}
		}


		// If the current node's event code didn't change the graph's state, then we check for
		// static event transitions.
		if (this->TRANSITION.Valid(TID)) {
			if (CurrentState->Node) CurrentState->Node->EventWithData_Internal(EName, Data);
		}
	}
}

UStateNode* UStateMachine::FindNode(FName NodeName, ESearchResult& Branches) {
	if (this->Graph.Contains(NodeName)) {		
		auto Node = this->Graph[NodeName].Node;
		UE_LOG(LogTemp, Warning, TEXT("Contains the state, but node????"));

		if (Node) 
		{
			Branches = ESearchResult::Found;
		} 
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("State found, but node was null...????"));
			Branches = ESearchResult::NotFound;

			auto DefObj = Cast<UStateMachine>(this->GetClass()->GetDefaultObject());

			if (DefObj)
			{
				UE_LOG(LogTemp, Warning, TEXT("Looping through default Object"));
				for (auto StateData : DefObj->Graph)
				{
					UE_LOG(LogTemp, Warning, TEXT("-- StateName: %s"), *StateData.Key.ToString());
					if (StateData.Value.Node)
					{
						UE_LOG(LogTemp, Warning, TEXT("-- Default Object had a node."));
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("-- Default Object did not have a node."));
					}
					
				}
			}
		}

		return Node;
	}
	else 
	{
		Branches = ESearchResult::NotFound;
		return nullptr;
	}
	
}

void UStateMachine::StateChangeListen(const FStateChangeDispatcher& Callback) {
	this->StateChangeEvents.Add(Callback);

	if (this->CurrentStateName != NAME_None) {
		Callback.ExecuteIfBound(this->CurrentStateName, this->CurrentStateName);
	}
}

void UStateMachine::StateChangeObject(UObject* Obj) {
	if (Obj->GetClass()->ImplementsInterface(UStateChangeListenerInterface::StaticClass())) {
		auto UFn = Obj->FindFunction("Listen");

		if (UFn) {
			FStateChangeDispatcher Callback;
			Callback.BindUFunction(Obj, "Listen");

			this->StateChangeEvents.Add(Callback);

			if (this->CurrentStateName != NAME_None) {
				Callback.ExecuteIfBound(this->CurrentStateName, this->CurrentStateName);
			}
		}
	}
}

void UStateMachine::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
}

void UStateMachine::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);
}

FStateData* UStateMachine::GetStateData(FName Name)
{
	UE_LOG(LogTemp, Warning, TEXT("Attemping to get state data for state: %s"), *Name.ToString());
	return this->Graph.Find(Name);
}

void UStateMachine::InitFromArchetype()
{	
	UE_LOG(LogTemp, Warning, TEXT("Attempting InitFromArchetype."));
	if (UBlueprint* BlueprintAsset = UBlueprint::GetBlueprintFromClass(this->GetClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Finding Blueprint Assets fine?"));
		if (auto BMBPG = Cast<UStateMachineBlueprintGeneratedClass>(BlueprintAsset->GeneratedClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("Initializing data from generated class."));
			this->ArchetypeClass = BMBPG;
			this->MachineArchetype = BMBPG->StateMachineArchetype;
			this->StartState = this->MachineArchetype->StartState;

			for (auto& SubMachine : BMBPG->SubStateMachineArchetypes)
			{
				UStateMachine* DupMachine = NewObject<UStateMachine>(
					this, 
					SubMachine.Value->ArchetypeClass.Get());

				DupMachine->ParentMachine = this;
				DupMachine->MachineArchetype = SubMachine.Value;
				DupMachine->ParentKey = SubMachine.Key;

				this->SubMachines.Add(SubMachine.Key, DupMachine);

				DupMachine->Initialize_Internal(this->Owner);
			}
		}
	}
}

/* Simply iterates through the graph and rebinds condition callbacks. */
void UStateMachine::RebindConditions() {
	//UStateMachine* Callee = this->RootMachine.Get() ? this->RootMachine.Get() : this;
	//TArray<FString> ValidFunctions = this->ConditionOptions();
	
	for (auto& pairs : this->Graph) {
		for (auto& tpairs : pairs.Value.Transitions) {
			tpairs.Value.ConditionCallback.BindUFunction(this, tpairs.Value.Condition);
			tpairs.Value.DataConditionCallback.BindUFunction(this, tpairs.Value.DataCondition);
		}
	}
}

void UStateMachine::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
    Super::PostEditChangeProperty(e);
}

FName UStateMachine::GetStateName(UStateNode* Node) {
	FName Found = NAME_None;

	for (const auto& Nodes : this->Graph) {
		if (Nodes.Value.Node == Node) {
			Found = Nodes.Key;
			break;
		}
	}

	return Found;
}

FName UStateMachine::GetCurrentStateName() {
	return this->CurrentStateName;
}

TArray<FString> UStateMachine::StateOptions() {
	TArray<FString> Names;

	for (const auto& Nodes : this->Graph) {
		Names.Add(Nodes.Key.ToString());
	}
	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UStateMachine::ConditionOptions() {
	TArray<FString> Names;
	auto base = this->FindFunction("TrueCondition");

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;
		
		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}		
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UStateMachine::ConditionDataOptions() {
	TArray<FString> Names;
	auto base = this->FindFunction("TrueDataCondition");

	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;		
		
		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

bool UStateMachine::TrueCondition() { 
	return true; 
}

bool UStateMachine::FalseCondition() {
	return false;
}

bool UStateMachine::TrueDataCondition(UObject* Data) {
	return true;
}

bool UStateMachine::FalseDataCondition(UObject* Data) {
	return false;
}

bool UStateMachine::ValidDataCondition(UObject* Data) {
	return Data != nullptr;
}

TSet<FName> UStateMachine::GetEvents() const {
	TSet<FName> List;

	for (const auto& States : this->Graph) {
		for (const auto& Event : States.Value.Transitions) {
			List.Add(Event.Key);

			if (!States.Value.Node) {
				States.Value.Node->GetEvents(List);
			}
		}
	}

	return List;
}

void UStateMachine::ValidateEventProps() {
	if (UBlueprint* BlueprintAsset = UBlueprint::GetBlueprintFromClass(this->GetClass())) {
		auto Events = this->GetEvents();


		for (const auto& EName : Events) {
			auto VName = this->GetEventVarName(EName);
			this->AddEventRefStruct(BlueprintAsset, VName, EName);
		}

		//FBlueprintEditorUtils::RefreshVariables(BlueprintAsset);
	}
}

bool UStateMachine::HasEventVariable(FName VName) {
	auto Prop = this->GetClass()->FindPropertyByName(VName);

	if (Prop) {
		auto StructProp = CastField<FStructProperty>(Prop);

		if (StructProp->Struct == FStateMachineEventRef::StaticStruct()) {
			return true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Found Conflicting variable. Cannot generate event property."));
			return true;
		}
	}

	return false;
}

FName UStateMachine::GetEventVarName(FName EName) {
	return FName(EName.ToString() + FString("_SM_EVENT"));
}

void UStateMachine::AddEventRefStruct(UBlueprint* BlueprintAsset, FName VName, FName EName) {
	/*

	FEdGraphPinType PinType;
	PinType.ContainerType = EPinContainerType::None;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FStateMachineEventRef::StaticStruct();

	if (FBlueprintEditorUtils::AddMemberVariable(BlueprintAsset, VName, PinType)) {
		const int32 VarIndex =
			FBlueprintEditorUtils::FindNewVariableIndex(BlueprintAsset, VName);

		if (VarIndex != INDEX_NONE)
		{

			auto& Variable = BlueprintAsset->NewVariables[VarIndex];
			Variable.PropertyFlags |= CPF_EditConst;
			Variable.PropertyFlags &= ~CPF_Edit;
			Variable.PropertyFlags |= CPF_BlueprintReadOnly;

			Variable.Category = FText::FromString("Generated|Events");

			//Variable.DefaultValue = FString("FStateMachineEventRef(\"") + EName.ToString() + FString("\")");


			FBlueprintEditorUtils::SetVariableSaveGameFlag(BlueprintAsset, VName, false);
		}
	}
	*/
}

UStateNode* UStateMachine::GetCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches) {
	auto Node = this->GetCurrentState();
	if (Class.Get() && Node) {		
		if (Node->Node && Node->Node->IsA(Class.Get()->StaticClass())) {
			Branches = ESearchResult::Found;

			return Node->Node;
		}
	}
	Branches = ESearchResult::NotFound;
	return nullptr;
}

FStateData* UStateMachine::GetCurrentState()
{
	//UE_LOG(LogTemp, Warning, TEXT("Getting Current State: %s"), *this->CurrentStateName.ToString());

	auto FoundData = this->Graph.Find(this->CurrentStateName);

	if (FoundData)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Found State Data already..."));
		return FoundData;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting GC copying."));
		auto SourceMachine = this->MachineArchetype.Get() ? this->MachineArchetype.Get() : this;

		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ArchetypeClass.Get()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Acquiring data from BPGC"));
			FStateData Data;
			bool Found = BPGC->GetStateData(Data, this, NAME_None, this->CurrentStateName);			

			if (Found)
			{
				if (this->ParentMachine && this->ParentMachine->ArchetypeClass)
				{
					auto SupBPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ParentMachine->ArchetypeClass.Get());
					FStateData SubData;
					bool SubFound = SupBPGC->GetStateData(SubData, this, this->ParentKey, this->CurrentStateName);
					

					if (SubFound && SubData.Node && Data.Node)
					{
						auto ANode = NewObject<UArrayNode>(this);
						ANode->AddNode(Data.Node);
						ANode->AddNode(SubData.Node);

						Data.Node = ANode;
					}
				}

				this->Graph.Add(this->CurrentStateName, Data);

				Data.Node->Initialize_Internal(this);

				return this->Graph.Find(this->CurrentStateName);
			}
		}
	}

	return nullptr;
}


#pragma endregion

#pragma region NodeCode

void UStateNode::Initialize_Internal(UStateMachine* POwner) {
	this->Owner = POwner;
	this->Initialize();
}

void UStateNode::Initialize_Implementation() {}


UStateMachine* UStateNode::GetMachine() {
	return this->Owner;
}

void UStateNode::GoTo(FName State) {
	this->Owner->UpdateState(State);
}

AActor* UStateNode::GetOwner() {
	return this->Owner->GetOwner();
}

void UStateNode::Event_Internal(FName EName) {
	if (this->bActive) {
		this->Event(EName);
	}
}

void UStateNode::Event_Implementation(FName EName) {
	// Does Nothing by default.
}

void UStateNode::EventWithData_Internal(FName EName, UObject* Data) {
	if (this->bActive) {
		this->EventWithData(EName, Data);
	}
}

void UStateNode::EventWithData_Implementation(FName EName, UObject* Data) {
	this->Event(EName);
}

void UStateNode::SetOwner(UStateMachine* Parent) {
	this->Owner = Parent;
}

FName UStateNode::GetStateName() {
	FName Found = NAME_None;

	if (this->Owner) {
		return this->Owner->GetStateName(this);
	}
	else {
		return NAME_None;
	}
	
}

void UStateNode::Tick_Internal(float DeltaTime) {
	if (this->bActive) {
		this->Tick(DeltaTime);
	}
}

void UStateNode::Exit_Internal() {
	if (this->bActive) {
		this->bActive = false;
		this->Exit();
	}	
}

void UStateNode::ExitWithData_Internal(UObject* Data) {
	if (this->bActive) {
		this->bActive = false;
		this->ExitWithData(Data);
	}
}

void UStateNode::ExitWithData_Implementation(UObject* Data) {
	this->Exit();
}

void UStateNode::Enter_Internal() {
	if (!this->bActive) {
		this->bActive = true;
		this->Enter();
	}
}

void UStateNode::EnterWithData_Internal(UObject* Data) {
	if (!this->bActive) {
		this->bActive = true;
		this->EnterWithData(Data);
	}
}

void UStateNode::EnterWithData_Implementation(UObject* Data) {
	this->Enter();
}

void UStateNode::GetEvents(TSet<FName>& List) {

}


#pragma endregion


void FStateMachineEventRef::Activate() {
	if (this->Owner.IsValid()) {
		this->Owner->Event(this->EventName);
	}
}

void FStateMachineEventRef::ActivateWithData(UObject* Data) {
	if (this->Owner.IsValid()) {
		this->Owner->EventWithData(this->EventName, Data);
	}
}
