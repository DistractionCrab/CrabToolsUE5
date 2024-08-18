#include "StateMachine/StateMachine.h"

#include "Logging/StructuredLog.h"
#include "Algo/Reverse.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/ArrayNode.h"
#include "Utils/UtilsLibrary.h"
#include "StateMachine/IStateMachineLike.h"

#pragma region StateMachine Code

UStateMachine::UStateMachine(const FObjectInitializer& ObjectInitializer)
{
	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		for (auto SubMachine : BPGC->SubStateMachineArchetypes)
		{
			// Class can be null for default objects.
			if (!SubMachine.Value->ArchetypeClass.Get()) { continue; }

			auto NewMachine = NewObject<UStateMachine>(this, SubMachine.Value->ArchetypeClass.Get(), SubMachine.Key);
			NewMachine->ParentMachine = this;
			NewMachine->ParentKey = SubMachine.Key;
			NewMachine->MachineArchetype = SubMachine.Value;

			this->SubMachines.Add(SubMachine.Key, NewMachine);
		}
	}
}

void UStateMachine::Initialize_Internal(AActor* POwner)
{
	this->Owner = POwner;
	this->InitFromArchetype();
	this->Initialize();

	for (auto SubMachine : this->SubMachines)
	{
		SubMachine.Value->Initialize_Internal(POwner);
	}

	// Shared nodes always exist, and should be initialize from the beginning.
	for (auto& Node : this->SharedNodes)
	{
		Node.Value->Initialize_Internal(this);
	}

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

void UStateMachine::AddTransition(FName State, FName Event, FName Destination, FName Condition, FName DataCondition)
{
	if (auto StateData = this->Graph.Find(State))
	{
		FTransitionData Data =
		{
			Destination,
			Condition,
			DataCondition
		};
		StateData->Transitions.Add(Event, Data);
	}
}

void UStateMachine::AddTransition(FName State, FName Event, FTransitionData Data)
{
	if (auto StateData = this->Graph.Find(State))
	{
		StateData->Transitions.Add(Event, Data);
	}
}

void UStateMachine::UpdateState(FName Name) {
	if (Name != this->CurrentStateName) {
		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		auto TID = this->TRANSITION.EnterTransition();
		// Always exit on the node, regardless of further state transitions.
		// If there were any updates to the state prior to this, then Exit will have
		// called already, and this function will have returned;
		if (CurrentState && CurrentState->Node) CurrentState->Node->Exit_Internal();

		if (this->TRANSITION.Valid(TID)) {
			this->CurrentStateName = Name;
			this->PushStateToStack(Name);
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
			this->PushStateToStack(Name);
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

void UStateMachine::Event_Direct(FName EName) {
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


void UStateMachine::EventWithData_Direct(FName EName, UObject* Data) {
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

#if WITH_EDITOR
void UStateMachine::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);
}

void UStateMachine::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
	Super::PostEditChangeChainProperty(e);
}

void UStateMachine::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
    Super::PostEditChangeProperty(e);
}
#endif

FStateData* UStateMachine::GetStateData(FName Name)
{
	return this->Graph.Find(Name);
}


void UStateMachine::InitFromArchetype()
{	
	if (auto BMBPG = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		this->ArchetypeClass = BMBPG;
		this->MachineArchetype = BMBPG->StateMachineArchetype;
		this->StartState = this->MachineArchetype->StartState;

		for (auto& SubMachine : BMBPG->SubStateMachineArchetypes)
		{
			if (!SubMachine.Value->ArchetypeClass)
			{
				UE_LOG(LogTemp, Error, TEXT("Sub-StateMachine source class was null for: %s"),
					*SubMachine.Key.ToString());
			}

			// If it's already been initialized from the constructor ignore this one.
			if (this->SubMachines.Contains(SubMachine.Key)) { continue; }

			UStateMachine* DupMachine = NewObject<UStateMachine>(
				this,
				SubMachine.Value->ArchetypeClass.Get());

			DupMachine->ParentMachine = this;
			DupMachine->MachineArchetype = SubMachine.Value;
			DupMachine->ParentKey = SubMachine.Key;

			this->SubMachines.Add(SubMachine.Key, DupMachine);

			// Do not initialize here. This will be done elsewhere.
			//DupMachine->Initialize_Internal(this->Owner);
		}
	}
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

FName UStateMachine::GetCurrentStateName()
{
	return this->CurrentStateName;
}

TArray<FString> UStateMachine::StateOptions()
{
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
		}
	}

	return List;
}

bool UStateMachine::HasEventVariable(FName VName) {
	auto Prop = this->GetClass()->FindPropertyByName(VName);

	if (Prop) {
		auto StructProp = CastField<FStructProperty>(Prop);

		if (StructProp->Struct == FStateMachineEventRef::StaticStruct()) {
			return true;
		}
		else {
			return true;
		}
	}

	return false;
}

FName UStateMachine::GetEventVarName(FName EName) {
	return FName(EName.ToString() + FString("_SM_EVENT"));
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

void SplitConditionPath(FName ConditionName, FString& OutParent, FString& ConditionString)
{
	ConditionName.ToString().Split("/", &OutParent, &ConditionString);
}

FStateData* UStateMachine::GetCurrentState()
{
	auto FoundData = this->Graph.Find(this->CurrentStateName);

	if (FoundData)
	{
		return FoundData;
	}
	else
	{
		auto SourceMachine = this->MachineArchetype.Get() ? this->MachineArchetype.Get() : this;

		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ArchetypeClass.Get()))
		{
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

				for (auto& tpairs : Data.Transitions)
				{
					this->BindCondition(tpairs.Value);
				}

				this->Graph.Add(this->CurrentStateName, Data);

				if (IsValid(Data.Node))
				{
					Data.Node->Initialize_Internal(this);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Null node for state: %s"), *this->CurrentStateName.ToString());
				}

				return this->Graph.Find(this->CurrentStateName);
			}
		}
	}

	return nullptr;
}

void UStateMachine::BindCondition(FTransitionData& Data)
{
	FString RootPath;
	FString ConditionPath;
	SplitConditionPath(Data.Condition, RootPath, ConditionPath);

	if (RootPath == "..")
	{
		if (this->ParentMachine)
		{
			Data.ConditionCallback.BindUFunction(this->ParentMachine, FName(ConditionPath));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Found Condition for parent machine with null parent: %s"),
				*Data.Condition.ToString());
		}
	}
	else
	{
		Data.ConditionCallback.BindUFunction(this, Data.Condition);
	}


	Data.DataConditionCallback.BindUFunction(this, Data.DataCondition);
}

void UStateMachine::PushStateToStack(FName EName)
{
	if (this->StateStack.Num() >= this->MaxStackSize)
	{
		this->StateStack.RemoveNode(this->StateStack.GetHead());
	}

	this->StateStack.AddTail(EName);
}

FName UStateMachine::GetPreviousState() const
{
	if (this->StateStack.Num() <= 1)
	{
		return NAME_None;
	}
	else
	{
		return this->StateStack.GetTail()->GetPrevNode()->GetValue();
	}
}

#pragma endregion

#pragma region NodeCode

UStateNode::UStateNode()
{

}

void UStateNode::Initialize_Internal(UStateMachine* POwner) {
	this->Owner = POwner;
	this->Initialize();
}

void UStateNode::Initialize_Implementation() {}


UStateMachine* UStateNode::GetMachine() {
	return this->Owner;
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

void UStateNode::DeleteEvent_Implementation(FName Event)
{
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FStructProperty* f = *FIT;

		if (f->Struct == FEventSlot::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FEventSlot>(this);
			if (Value->EventName == Event)
			{
				Value->EventName = NAME_None;
			}
		}
	}
}

void UStateNode::RenameEvent_Implementation(FName From, FName To)
{
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FStructProperty* f = *FIT;

		if (f->Struct == FEventSlot::StaticStruct())
		{
			auto Value = f->ContainerPtrToValuePtr<FEventSlot>(this);
			if (Value->EventName == From)
			{
				Value->EventName = To;
			}
		}
	}
}

void UStateNode::EmitEvent(FName EName) { this->GetMachine()->Event_Direct(EName); }
void UStateNode::EmitEventSlot(const FEventSlot& ESlot) { this->GetMachine()->Event_Direct(ESlot.EventName); }

void UStateNode::EmitEventWithData(FName EName, UObject* Data) { this->GetMachine()->EventWithData_Direct(EName, Data); }
void UStateNode::EmitEventSlotWithData(const FEventSlot& ESlot, UObject* Data) { this->GetMachine()->EventWithData_Direct(ESlot.EventName, Data); }

#if WITH_EDITOR
void UStateNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	/*
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UStateNode, EmittedEvents))
	{
		auto Removed = this->EmittedEvents.Difference(this->PreEditEmittedEvents);
		if (Removed.Num() > 0)
		{
			if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
			{
				auto Replaced = this->PreEditEmittedEvents.Difference(this->EmittedEvents).Array();
				if (Removed.Num() == 1 && Replaced.Num() == 1)
				{
					StateLike->RenameEvent(Replaced[0], Removed.Array()[0]);
				}
				else
				{
					for (auto EName : Removed)
					{
						StateLike->DeleteEvent(EName);
					}
				}
			}
		}		

		this->PreEditEmittedEvents.Empty();
	}
	*/
}

void UStateNode::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UStateNode, EmittedEvents))
	{
		for (auto EventName : this->EmittedEvents)
		{
			this->PreEditEmittedEvents.Add(EventName);
		}
	}
}

#endif

#if WITH_EDITORONLY_DATA
TArray<FString> UStateNode::GetEventOptions() const
{
	if (auto StateLike = UtilsFunctions::GetOuterAs<IStateLike>(this))
	{
		return StateLike->GetEventOptions();
	}

	return { FName(NAME_None).ToString() };
}

void UStateNode::GetEmittedEvents(TSet<FName>& Events) const 
{
	Events.Append(this->EmittedEvents); 
}
#endif

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
