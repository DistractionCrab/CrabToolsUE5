#include "StateMachine/EventMapNode.h"

void UEventMapNode::Event_Implementation(FName Event) {
	if (this->EventMapInternal.Contains(Event)) {
		auto h = this->EventMapInternal[Event];
		if (h.IsBound()) {
			h.Execute(Event);
		}
	}
}

void UEventMapNode::Initialize_Implementation() {
	for (const auto& Pair : this->EventMap) {
		FEventHandler f;
		f.BindUFunction(this, Pair.Value);
		this->EventMapInternal.Add(Pair.Key, f);
	}
}

TArray<FString> UEventMapNode::HandlerNameOptions() {

	TArray<FString> Names;
	for (TFieldIterator<UFunction> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;

		auto prop = f->GetReturnProperty();
		if (!prop) {
			TArray<FProperty*> Params;
			for (auto PIT = TFieldIterator<FProperty>(f); PIT; ++PIT) {
				Params.Add(*PIT);
			}		

			if (Params.Num() == 1) {
				bool c1 = Params[0]->IsA(FNameProperty::StaticClass());
				bool c2 = f->GetName() != "GoTo";
				bool c3 = f->GetName() != "Event";

				if (c1 && c2 && c3) {
					Names.Add(f->GetName());
				}
			}
		}		
	}

	return Names;
}