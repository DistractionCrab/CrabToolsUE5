#include "StateMachine/General/SetBooleanNode.h"
#include "Utils/UtilsLibrary.h"

void USetBooleanNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	FString Address = this->PropertyName.ToString();
	FSMPropertySearch Params = FSMPropertySearch::Property(FBoolProperty::StaticClass());
	this->PropertyRef = Params.GetProperty<FBoolProperty>(this->GetMachine(), Address);
}

void USetBooleanNode::Exit_Inner_Implementation()
{
	this->PropertyRef.SetValue<bool>(this->bOnExit);
}


void USetBooleanNode::Enter_Inner_Implementation()
{
	this->PropertyRef.SetValue<bool>(this->bOnEnter);
}

#if WITH_EDITOR
TArray<FString> USetBooleanNode::GetPropertyOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		FSMPropertySearch Params = FSMPropertySearch::Property(FBoolProperty::StaticClass());

		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}
#endif
