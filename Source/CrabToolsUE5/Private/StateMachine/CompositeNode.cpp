// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/CompositeNode.h"


void UCompositeNode::Tick_Implementation(float DeltaTime) {
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Tick(DeltaTime);	
		}
		if (!this->active) {
			break;
		}
	}
}

void UCompositeNode::Event_Implementation(FName Event) {
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Event(Event);	
		}
		if (!this->active) {
			break;
		}
	}
}

void UCompositeNode::Enter_Implementation() {
	this->active = true;
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Enter();	
		}
	}
}

void UCompositeNode::Exit_Implementation() {
	this->active = false;
	for (const auto& Node : this->Nodes) {
		if (Node) {
			Node->Exit();	
		}
	}
}