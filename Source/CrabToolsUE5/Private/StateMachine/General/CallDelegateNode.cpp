#include "StateMachine/General/CallDelegateNode.h"
#include "Utils/UtilsLibrary.h"

void UCallDelegateNode::Initialize_Inner_Implementation()
{
	Super::Initialize_Inner_Implementation();

	FSMPropertySearch Params = FSMPropertySearch::InlineDelegate(
		this->FindFunction(GET_FUNCTION_NAME_CHECKED(UCallDelegateNode, DelegateSignatureFunction)));

	FString EnterAddress = this->OnEnterDelegate.ToString();
	this->OnEnterRef = Params.GetProperty<FMulticastInlineDelegateProperty>(this->GetMachine(), EnterAddress);

	FString ExitAddress = this->OnExitDelegate.ToString();
	this->OnEnterRef = Params.GetProperty<FMulticastInlineDelegateProperty>(this->GetMachine(), ExitAddress);
}

void UCallDelegateNode::Exit_Inner_Implementation()
{
	if (auto Del = this->OnExitRef.GetValue<FMulticastScriptDelegate>())
	{
		Del->ProcessMulticastDelegate<UObject>(nullptr);
	}
}


void UCallDelegateNode::Enter_Inner_Implementation()
{
	if (auto Del = this->OnEnterRef.GetValue<FMulticastScriptDelegate>())
	{
		Del->ProcessMulticastDelegate<UObject>(nullptr);
	}
}

#if WITH_EDITOR
TArray<FString> UCallDelegateNode::GetPropertyOptions() const
{
	TArray<FString> Props;

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		FSMPropertySearch Params = FSMPropertySearch::InlineDelegate(
			this->FindFunction(GET_FUNCTION_NAME_CHECKED(UCallDelegateNode, DelegateSignatureFunction)));

		Props.Append(Outer->GetPropertiesOptions(Params));
	}

	Props.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Props;
}
#endif
