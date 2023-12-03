#include "Utils/UtilsLibrary.h"

void UUtilsLibrary::ActivateTimeGatedBool(FTimeGatedBool& input) {
	input.Reset();
}

bool UUtilsLibrary::TimeGatedBoolConvert(const FTimeGatedBool& input) {
	return input.GetValue();
}