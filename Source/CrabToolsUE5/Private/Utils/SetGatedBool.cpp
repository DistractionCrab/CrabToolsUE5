#include "Utils/SetGatedBool.h"
#include "Engine/World.h"
#include "Engine.h"


bool FSetGatedBool::ContainsObj(UObject* Obj) const {
	return this->Objects.Contains(Obj);
}

void FSetGatedBool::AddObj(UObject* Obj) {
	this->Objects.Add(Obj);
}