#include "Utils/UtilsLibrary.h"

void UUtilsLibrary::ActivateTimeGatedBool(FTimeGatedBool& input) {
	input.Reset();
}

bool UUtilsLibrary::TimeGatedBoolConvert(const FTimeGatedBool& input) {
	return input.GetValue();
}
float UUtilsLibrary::RotateAngleTo(float Base, float Goal, float Delta) {
	// Normalize the angles to [-180, 180] and to [0, 360].
	Base = FRotator::NormalizeAxis(Base);
	Goal = FRotator::NormalizeAxis(Goal);
	float DeBase = Base < 0 ? Base + 360 : Base;
	float DeGoal = Goal < 0 ? Goal + 360 : Goal;

	// Find the relative diffs for different ranges.
	float DiffNorm = Goal - Base;
	float DiffUnNorm = DeGoal - DeBase;

	// Whichever is the smallest diff will be the true difference in the angle.
	float Diff = FMath::Abs(DiffNorm) < FMath::Abs(DiffUnNorm) ? DiffNorm : DiffUnNorm;

	if (FMath::Abs(Diff) < FMath::Abs(Delta)) {
		return Goal;
	}
	else {
		return Base + FMath::Sign(Diff)*Delta;
	}
}

bool UUtilsLibrary::Contains(const FSetGatedBool& Input, UObject* Obj) {
	return Input.ContainsObj(Obj);
}

void UUtilsLibrary::GateObj(FSetGatedBool& Input, UObject* Obj) {
	Input.AddObj(Obj);
}