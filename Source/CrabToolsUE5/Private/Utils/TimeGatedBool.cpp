#include "Utils/TimeGatedBool.h"
#include "Engine/World.h"
#include "Engine.h"

void FTimeGatedBool::Reset() {
	this->Value = true;

	if (GEngine) {
		FTimerDelegate TD;
		TD.BindLambda([this] {this->Done(); });
		GEngine->GetWorld()->GetTimerManager().SetTimer(
			this->Timer,
			TD,
			this->Time,
			false,
			0.0
		);
	}
}

void FTimeGatedBool::Done() {
	this->Value = false;

	if (GEngine) {
		GEngine->GetWorld()->GetTimerManager().ClearTimer(this->Timer);
	}
}

bool FTimeGatedBool::GetValue() const {
	return this->Value;
}