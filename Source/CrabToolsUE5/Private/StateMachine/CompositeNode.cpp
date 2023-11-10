// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/CompositeNode.h"

void UCompositeNode::Initialize_Implementation(UProcStateMachine* POwner) {
	Super::Initialize_Implementation(POwner);
	for (auto& NodePairs : this->Nodes) {
		NodePairs.Value->Initialize_Internal(POwner);
	}
}

void UCompositeNode::Tick_Implementation(float DeltaTime) {
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Tick(DeltaTime);	
		}
		if (!this->active) {
			break;
		}
	}
}

void UCompositeNode::Event_Implementation(FName Event) {
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Event(Event);	
		}
		if (!this->active) {
			break;
		}
	}
}

void UCompositeNode::Enter_Implementation() {
	this->active = true;
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Enter();	
		}
	}
}

void UCompositeNode::Exit_Implementation() {
	this->active = false;
	for (const auto& Node : this->Nodes) {
		if (Node.Value) {
			Node.Value->Exit();	
		}
	}
}

UStateNode* UCompositeNode::FindNodeByArray_Implementation(const TArray<FString>& Path, ENodeSearchResult& Branches) {
	if (Path.Num() == 0) {
		Branches = ENodeSearchResult::NOTFOUND;
		return nullptr;
	}
	else {
		FName Name(Path.Last());
		if (this->Nodes.Contains(Name)) {
			if (Path.Num() == 1) {
				Branches = ENodeSearchResult::FOUND;
				return this->Nodes[Name];
			}
			else {
				TArray<FString> Tail(Path);
				Tail.Pop();
				return this->Nodes[Name]->FindNodeByArray(Tail, Branches);
			}
		}
		else {
			Branches = ENodeSearchResult::NOTFOUND;
			return nullptr;
		}
	}
}