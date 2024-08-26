#include "StateMachine/IStateMachineLike.h"

bool FSMPropertySearch::Matches(FProperty* F) const
{
	// If the variable isn't public, then do not expose it yourself.
	if (!(F->GetFlags() & EObjectFlags::RF_Public)) { return false; }

	if (F->GetClass() == this->FClass)
	{
		if (F->GetClass() == FStructProperty::StaticClass())
		{
			FStructProperty* Sf = (FStructProperty*) F;

			if (Sf->Struct == this->Struct)
			{
				return true;
			}
		}
		else if (F->GetClass() == FObjectProperty::StaticClass())
		{
			FObjectProperty* Of = (FObjectProperty*) F;

			if (Of->PropertyClass == this->Class)
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}