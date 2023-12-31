#include "Components/RPGSystem/RPGHelpers.h"

#pragma region Integer Attributes and Resources
void URPGHelpers::ObserveIntAttribute(UPARAM(ref) FIntAttribute& input, const FIntAttributeCallback& Callback) {
	input.IntChangedEvent.Add(Callback);
}

void URPGHelpers::ObserveIntResource(UPARAM(ref) FIntResource& input, const FIntAttributeCallback& Callback) {
	input.IntChangedEvent.Add(Callback);
}

void URPGHelpers::SetIntResource(UPARAM(ref) FIntResource& input, int Value) {
	input.SetValue(Value);
}

int URPGHelpers::UnwrapIntAttribute(const FIntAttribute& input) {
	return input.GetValue();
}

int URPGHelpers::UnwrapIntResource(const FIntResource& input) {
	return input.GetValue();
}


int URPGHelpers::MaxIntRescValue(const FIntResource& Input) {
	return Input.GetMax();
}


int URPGHelpers::MinIntRescValue(const FIntResource& Input) {
	return Input.GetMin();
}

float URPGHelpers::IntRescPercent(const FIntResource& Input) {
	return Input.GetPercent();
}

#pragma endregion


#pragma region Float Attributes and Resources

void URPGHelpers::ObserveFloatAttribute(UPARAM(ref) FFloatAttribute& input, const FFloatAttributeCallback& Callback) {
	input.FloatChangedEvent.Add(Callback);
}

void URPGHelpers::ObserveFloatResource(UPARAM(ref) FFloatResource& input, const FFloatAttributeCallback& Callback) {
	input.FloatChangedEvent.Add(Callback);
}

void URPGHelpers::SetFloatResource(UPARAM(ref) FFloatResource& input, float Value) {
	input.SetValue(Value);
}

float URPGHelpers::UnwrapFloatAttribute(const FFloatAttribute& input) {
	return input.GetValue();
}

float URPGHelpers::UnwrapFloatResource(const FFloatResource& input) {
	return input.GetValue();
}

float URPGHelpers::MaxFloatRescValue(const FFloatResource& Input) {
	return Input.GetMax();
}

float URPGHelpers::MinFloatRescValue(const FFloatResource& Input) {
	return Input.GetMin();
}

float URPGHelpers::FloatRescPercent(const FFloatResource& Input) {
	return Input.GetPercent();
}

#pragma endregion