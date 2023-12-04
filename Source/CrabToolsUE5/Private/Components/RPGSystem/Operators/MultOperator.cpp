#include "Components/RPGSystem/Operators/MultOperator.h"

int UIntMultOperator::Operate_Implementation(int Value) {
	return Value * this->Scalar;
}