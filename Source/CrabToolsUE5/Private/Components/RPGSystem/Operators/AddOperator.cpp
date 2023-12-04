#include "Components/RPGSystem/Operators/AddOperator.h"

int UIntAddOperator::Operate_Implementation(int Value) {
	return Value + this->Additive;
}