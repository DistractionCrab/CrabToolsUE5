#include "StateMachine/StateMachine.h"

#include "Logging/StructuredLog.h"
#include "Algo/Reverse.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/ArrayNode.h"
#include "StateMachine/Utils.h"
#include "StateMachine/Logging.h"
#include "StateMachine/IStateMachineLike.h"
#include "Utils/UtilsLibrary.h"

#pragma region StateMachine Code

UStateMachine::UStateMachine(const FObjectInitializer& ObjectInitializer)
{
	
}

void UStateMachine::InitSubMachines()
{
	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		for (auto& Key : BPGC->GetSubMachineOptions())
		{
			auto Machine = BPGC->ConstructSubMachine(this, Key);
			this->SubMachines.Add(Key, Machine);
		}
	}

	for (auto& SubMachine : this->SubMachines)
	{
		if (SubMachine.Value)
		{
			SubMachine.Value->Initialize_Internal(this->Owner);
		}
	}
}

void UStateMachine::Initialize_Internal(AActor* POwner)
{
	this->Owner = POwner;	
	this->InitFromArchetype();	
	this->Initialize();
	this->InitSubMachines();	

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
	auto State = NewObject<UState>(this);
	this->Graph.Add(StateName, State);
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
		StateData->Get()->Transitions.Add(Event, Data);
	}
}

void UStateMachine::AddTransition(FName State, FName Event, FTransitionData Data)
{
	if (auto StateData = this->Graph.Find(State))
	{
		StateData->Get()->Transitions.Add(Event, Data);
	}
}

void UStateMachine::UpdateState(FName Name)
{
	if (Name != this->CurrentStateName)
	{
		this->TransitionIdentifier.EnterTransition();
		this->bIsTransitioning = true;

		FStateChangedEventData StateChangedData;

		StateChangedData.StateMachine = this;
		StateChangedData.From = this->CurrentStateName;
		StateChangedData.To = Name;

		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		bool ValidState = CurrentState && CurrentState->Node;

		if (ValidState)
		{
			CurrentState->Node->Exit_Internal();
			StateChangedData.FromState = CurrentState;
		}


		this->CurrentStateName = Name;
		this->PushStateToStack(Name);
		CurrentState = this->GetCurrentState();

		ValidState = CurrentState && CurrentState->Node;

		if (ValidState)
		{
			StateChangedData.ToState = CurrentState;
			CurrentState->Node->Enter_Internal();
		}

		this->OnStateChanged.Broadcast(StateChangedData);

		this->bIsTransitioning = false;

		if (ValidState)
		{
			CurrentState->Node->PostTransition_Internal();
		}
		
		this->OnTransitionFinished.Broadcast(this);
	}
}

void UStateMachine::UpdateStateWithData(FName Name, UObject* Data)
{
	if (Name != this->CurrentStateName)
	{
		this->TransitionIdentifier.EnterTransition();
		this->bIsTransitioning = true;

		FStateChangedEventData StateChangedData;

		StateChangedData.StateMachine = this;
		StateChangedData.From = this->CurrentStateName;
		StateChangedData.To = Name;

		auto CurrentState = this->GetCurrentState();
		auto OldState = this->CurrentStateName;

		bool ValidState = CurrentState && CurrentState->Node;

		if (CurrentState && CurrentState->Node)
		{
			StateChangedData.FromState = CurrentState;
			CurrentState->Node->ExitWithData_Internal(Data);
		}

		this->CurrentStateName = Name;
		this->PushStateToStack(Name);
		CurrentState = this->GetCurrentState();

		ValidState = CurrentState && CurrentState->Node;

		if (ValidState)
		{
			StateChangedData.ToState = CurrentState;
			CurrentState->Node->EnterWithData_Internal(Data);
		}

		this->OnStateChanged.Broadcast(StateChangedData);
		this->bIsTransitioning = false;

		if (ValidState)
		{
			CurrentState->Node->PostTransition_Internal();
		}

		this->OnTransitionFinished.Broadcast(this);
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

void UStateMachine::SendEvent(FName EName)
{
	if (this->bIsTransitioning) { return; }

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
				UE_LOGFMT(LogStateMachine, Error, "Condition Delegate Unbound? Condition Name: {0}", TData.Condition);
			}
		}
		
		if (CurrentState->Node) CurrentState->Node->Event_Internal(EName);
	}	
}


void UStateMachine::SendEventWithData(FName EName, UObject* Data)
{
	if (this->bIsTransitioning) { return; }

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
				UE_LOGFMT(LogStateMachine, Error, "Condition Delegate Unbound? Condition Name: {0}", TData.Condition);
			}
		}

		if (CurrentState->Node) CurrentState->Node->EventWithData_Internal(EName, Data);
	}
}

void UStateMachine::SetActive(bool bNewActive)
{
	if (auto State = this->GetCurrentState())
	{
		if (State->GetNode())
		{
			State->GetNode()->SetActive(bNewActive);
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

void UStateMachine::PostLinkerChange()
{
	Super::PostLinkerChange();
}
#endif

UState* UStateMachine::GetStateData(FName Name)
{
	if (auto Data = this->Graph.Find(Name))
	{
		return Data->Get();
	}
	else
	{
		return nullptr;
	}
}


void UStateMachine::InitFromArchetype()
{	
	FName StartStateDefault = NAME_None;

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		StartStateDefault = BPGC->StateMachineArchetype->StartState;
	}

	if (IsValid(this->ParentMachine))
	{
		if (auto ParentBPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ParentMachine->GetClass()))
		{
			// If these two states aren't equal, then an override for the StartState happened in the editor.
			if (StartStateDefault == this->StartState)
			{
				if (auto Machine = ParentBPGC->SubStateMachineArchetypes[this->ParentKey])
				{
					this->StartState = Machine->StartState;
				}
			}
		}
	}
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

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
	{
		Names.Append(BPGC->StateMachineArchetype->StateOptions());
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

bool UStateMachine::TrueDataCondition(UObject* Data)
{
	return true;
}

bool UStateMachine::FalseDataCondition(UObject* Data)
{
	return false;
}

bool UStateMachine::ValidDataCondition(UObject* Data)
{
	return IsValid(Data);
}

TSet<FName> UStateMachine::GetEvents() const {
	TSet<FName> List;

	for (const auto& States : this->Graph) {
		for (const auto& Event : States.Value->Transitions) {
			List.Add(Event.Key);
		}
	}

	return List;
}

bool UStateMachine::HasEventVariable(FName VName) const {
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

void UStateMachine::AddStateData(FName StateName, UState* Data)
{
	if (Data->GetOuter() != this)
	{
		Data = DuplicateObject(Data, this);
	}

	if (!this->Graph.Contains(StateName))
	{
		this->Graph.Add(StateName, Data);
	}
}

void UStateMachine::AddStateWithNode(FName StateName, UStateNode* Node)
{
	UState* Data = NewObject<UState>(this);
	Data->Node = Cast<UStateNode>(DuplicateObject(Node, this));
	this->Graph.Add(StateName, Data);
}


UState* UStateMachine::GetCurrentState()
{
	auto FoundData = this->Graph.Find(this->CurrentStateName);

	if (FoundData)
	{
		return FoundData->Get();
	}
	else
	{
		UState* BuiltState = nullptr;

		// First get the data for this state from the Generated class if it exists.
		if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass()))
		{			
			BuiltState = BPGC->GetStateData(this, NAME_None, this->CurrentStateName);
		}
		
		// Then check to see if there's a parent machine to get extra state data from.
		if (IsValid(this->ParentMachine))
		{
			if (auto ParentBPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->ParentMachine->GetClass()))
			{
				
				auto SubState = ParentBPGC->GetStateData(this, this->ParentKey, this->CurrentStateName);

				if (IsValid(SubState))
				{
					if (IsValid(BuiltState))
					{
						BuiltState->Append(SubState);
					}
					else
					{
						BuiltState = SubState;
					}
					
				}
			}
		}

		if (IsValid(BuiltState))
		{
			this->Graph.Add(this->CurrentStateName, BuiltState);

			for (auto& tpairs : BuiltState->Transitions)
			{
				this->BindCondition(tpairs.Value);
			}			

			if (IsValid(BuiltState->Node))
			{
				BuiltState->Node->Initialize_Internal(this);
			}

			return BuiltState;
		}
		else
		{
			this->Graph.Add(this->CurrentStateName, NewObject<UState>());
		}
	}
	

	return nullptr;
}

TArray<FString> UStateMachine::GetPropertiesOptions(FSMPropertySearch& SearchParam) const
{
	TArray<FString> Names;

	for (TFieldIterator<FProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		FProperty* f = *FIT;

		if (SearchParam.Matches(f))
		{
			Names.Add(f->GetName());
		}
	}

	for (auto& SubMachine : this->SubMachines)
	{
		for (TFieldIterator<FProperty> FIT(SubMachine.Value->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FProperty* f = *FIT;

			if (SearchParam.Matches(f))
			{
				FString Formatted = FString::Printf(TEXT("%s/%s"), *SubMachine.Key.ToString(), *f->GetName());
				Names.Add(Formatted);
			}
		}
	}

	return Names;
}

IStateMachineLike* UStateMachine::GetSubMachine(FString& Address) const
{
	FString Base;
	FString Target;

	if (Address.Split(TEXT("/"), &Base, &Target))
	{
		FName BaseName(Base);

		if (auto SubM = this->SubMachines.Find(BaseName))
		{
			return SubM->Get();
		}
		else if (Base == "..")
		{
			if (this->ParentMachine)
			{
				return this->ParentMachine->GetSubMachine(Target);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		FName Key(Address);

		if (auto Found = this->SubMachines.Find(Key))
		{
			return Found->Get();
		}
		else if (!Key.IsNone())
		{
			UE_LOG(LogStateMachine, Warning, TEXT("Attempting retrieve null SubMachine: %s"), *Address);
			return nullptr;
		}
		else
		{
			return nullptr;
		}
	}
}

FProperty* UStateMachine::GetStateMachineProperty(FString& Address) const
{
	FString Base;
	FString Target;
	
	if (Address.Split(TEXT("/"), &Base, &Target))
	{
		FName BaseName(Base);

		if (auto SubM = this->SubMachines.Find(BaseName))
		{
			return SubM->Get()->GetStateMachineProperty(Address);
		}
		else if (Base == "..")
		{
			if (this->ParentMachine)
			{
				return this->ParentMachine->GetStateMachineProperty(Target);
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return this->GetClass()->FindPropertyByName(FName(Address));
	}
}

void UStateMachine::BindCondition(FTransitionData& Data)
{
	FString ConditionAddress = Data.Condition.ToString();
	FString DataConditionAddress = Data.DataCondition.ToString();
	this->BindConditionAt(ConditionAddress, Data);
	this->BindDataConditionAt(DataConditionAddress, Data);



}

void UStateMachine::BindConditionAt(FString& Address, FTransitionData& Data)
{
	FString RootPath;
	FString ConditionPath;

	if (Address.Split("/", &RootPath, &ConditionPath))
	{
		if (RootPath == "..")
		{
			if (this->ParentMachine)
			{
				this->ParentMachine->BindConditionAt(ConditionPath, Data);
			}
			else
			{
				UE_LOG(LogStateMachine, Error, TEXT("Found Condition for parent machine with null parent: %s"),
					*Data.Condition.ToString());
			}
		}
		else
		{
			if (auto SubPtr = this->SubMachines.Find(FName(RootPath)))
			{
				if (auto SubMachine = SubPtr->Get())
				{
					SubMachine->BindConditionAt(ConditionPath, Data);
				}
			}
		}
	}
	else
	{
		Data.ConditionCallback.BindUFunction(this, FName(Address));
	}
}

void UStateMachine::BindDataConditionAt(FString& Address, FTransitionData& Data)
{
	FString RootPath;
	FString ConditionPath;

	if (Address.Split("/", &RootPath, &ConditionPath))
	{
		if (RootPath == "..")
		{
			if (this->ParentMachine)
			{
				this->ParentMachine->BindDataConditionAt(ConditionPath, Data);
			}
			else
			{
				UE_LOG(LogStateMachine, Error, TEXT("Found Condition for parent machine with null parent: %s"),
					*Data.Condition.ToString());
			}
		}
		else
		{
			if (auto SubPtr = this->SubMachines.Find(FName(RootPath)))
			{
				if (auto SubMachine = SubPtr->Get())
				{
					SubMachine->BindDataConditionAt(ConditionPath, Data);
				}
			}
		}
	}
	else
	{
		Data.DataConditionCallback.BindUFunction(this, FName(Address));
	}
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

void UStateMachine::SetParentData(UStateMachine* Parent, FName NewParentKey)
{
	this->ParentMachine = Parent;
	this->ParentKey = NewParentKey;
}

UStateMachineBlueprintGeneratedClass* UStateMachine::GetGeneratedClass() const
{
	return Cast<UStateMachineBlueprintGeneratedClass>(this->GetClass());
}

TArray<FString> UStateMachine::GetStateOptions(const UObject* Asker) const
{
	TArray<FString> Names;

	for (auto& State : this->Graph)
	{
		auto Access = State.Value->Access;

		if (Asker)
		{
			if (Asker->IsIn(this) && StateMachineAccessibility::IsChildVisible(Access))
			{
				Names.Add(State.Key.ToString());
			}
			else if (Access == EStateMachineAccessibility::PUBLIC)
			{
				Names.Add(State.Key.ToString());
			}
		}
		else if (Access == EStateMachineAccessibility::PUBLIC)
		{
			Names.Add(State.Key.ToString());
		}
	}

	if (auto BPGC = this->GetGeneratedClass())
	{
		if (Asker)
		{
			if (Asker->IsIn(this))
			{
				Names.Append(BPGC->GetStateOptions(EStateMachineAccessibility::PROTECTED));
			}
			else
			{
				Names.Append(BPGC->GetStateOptions(EStateMachineAccessibility::PUBLIC));
			}
		}
		else
		{
			Names.Append(BPGC->GetStateOptions(EStateMachineAccessibility::PUBLIC));
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UStateMachine::GetStatesWithAccessibility(EStateMachineAccessibility Access) const
{
	TArray<FString> Names;

	for (auto State : this->Graph)
	{
		if (State.Value->Access == Access)
		{
			Names.Add(State.Key.ToString());
		}
	}

	return Names;
}

#pragma endregion

#pragma region NodeCode

UStateNode::UStateNode()
{

}

void UStateNode::Initialize_Internal(UStateMachine* POwner) {
	if (POwner)
	{
		this->Owner = POwner;
		this->Initialize();
	}
	else
	{
		UE_LOG(LogStateMachine, Error, TEXT(""));
	}
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


void UStateNode::Tick_Internal(float DeltaTime) {
	if (this->bActive) {
		this->Tick(DeltaTime);
	}
}

void UStateNode::PostTransition_Internal()
{
	if (this->bActive)
	{
		this->PostTransition();
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

bool UStateNode::Verify(FNodeVerificationContext& Context) const
{
	bool bErrorFree = true;

	if (auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
		{
			FStructProperty* f = *FIT;

			if (f->Struct == FEventSlot::StaticStruct())
			{

			}
			else if (f->Struct == FSubMachineSlot::StaticStruct())
			{
				FSubMachineSlot Value;
				f->GetValue_InContainer(this, &Value);

				auto Options = SMLike->GetMachineOptions();

				if (!(Value.MachineName.IsNone() || Options.Contains(Value.MachineName)))
				{
					FString Msg = FString::Printf(TEXT("Could not find StateMachine for slot %s: %s"),
						*f->GetName(),
						*this->GetName());
					Context.Error(Msg, this);

					bErrorFree = false;
				}
			}
		}
	}
	else
	{
		FString Msg = FString::Printf(TEXT("Could not found outer ISMLike for Node: %s"), *this->GetName());
		Context.Error(Msg, this);

		bErrorFree = false;
	}

	return bErrorFree;
}

void UStateNode::EmitEvent(FName EName)
{
	this->GetMachine()->SendEvent(EName); 
}

void UStateNode::EmitEventSlot(const FEventSlot& ESlot)
{ 
	this->GetMachine()->SendEvent(ESlot.EventName);
}

void UStateNode::EmitEventWithData(FName EName, UObject* Data) {
	this->GetMachine()->SendEventWithData(EName, Data);
}

void UStateNode::EmitEventSlotWithData(const FEventSlot& ESlot, UObject* Data) {
	this->GetMachine()->SendEventWithData(ESlot.EventName, Data);
}

#if WITH_EDITOR
void UStateNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
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

TArray<FString> UStateNode::GetMachineOptions() const
{
	const UObject* Outer = this;

	if (auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(Outer))
	{
		return SMLike->GetMachineOptions();
	}

	return { };
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

void UState::Append(UState* Data)
{
	this->AppendNode(Data->Node);
	this->Transitions.Append(Data->Transitions);
}

void UState::AppendCopy(UState* Data)
{
	this->AppendNodeCopy(Data->Node);
	this->Transitions.Append(Data->Transitions);
}

void UState::AppendNode(UStateNode* ANode)
{
	if (IsValid(ANode))
	{

		if (ANode->GetOuter() != this)
		{
			ANode = DuplicateObject(ANode, this);
		}

		if (auto ArrayNode = Cast<UArrayNode>(this->Node))
		{
			if (IsValid(ANode))
			{
				ArrayNode->AddNode(ANode);
			}
		}
		else
		{
			if (IsValid(this->Node))
			{
				auto NewNode = NewObject<UArrayNode>(this);
				NewNode->AddNode(this->Node);
				NewNode->AddNode(ANode);
				this->Node = NewNode;
			}
			else
			{
				this->Node = ANode;
			}
		}
	}
}

void UState::AppendNodeCopy(UStateNode* ANode)
{
	if (IsValid(ANode))
	{
		auto NewNode = DuplicateObject(ANode, this);
		this->AppendNode(NewNode);
	}
}