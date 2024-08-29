#include "CrabToolsUE5EditorModule.h"
#include "Selection.h"
#include "Actors/PatrolPath.h"

#define LOCTEXT_NAMESPACE "FCrabToolsUE5EditorModule"

const FName CrabToolsUE5EditorAppIdentifier = FName(TEXT("CrabToolsUE5EditorApp"));


FCrabToolsUE5EditorModule::FCrabToolsUE5EditorModule() {

}

void FCrabToolsUE5EditorModule::StartupModule() 
{
	UE_LOG(LogTemp, Warning, TEXT("Setting up on selection changed callback."));
	this->SelectionChangedHandle = USelection::SelectionChangedEvent.AddRaw(this, &FCrabToolsUE5EditorModule::OnSelectionChanged);
}

void FCrabToolsUE5EditorModule::ShutdownModule() 
{
	USelection::SelectionChangedEvent.Remove(SelectionChangedHandle);
}

FString FCrabToolsUE5EditorModule::GetReferencerName() const  {
	return "CrabToolsUE5EditorModule";
}


void FCrabToolsUE5EditorModule::OnSelectionChanged(UObject* Obj)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSelectionChanged called in module."));

	if (auto PatrolPath = Cast<APatrolPath>(Obj))
	{
		PatrolPath->ToggleDisplay();
	}

	if (auto Selection = Cast<USelection>(Obj))
	{
		TArray<UObject*> Selected;
		Selection->GetSelectedObjects(Selected);

		TSet<APatrolPath*> SelectedPaths;
		for (auto& SelectedObj : Selected)
		{
			UE_LOG(LogTemp, Warning, TEXT("- Selected object found: %s"), *SelectedObj->GetName());
			if (auto PatrolPath = Cast<APatrolPath>(SelectedObj))
			{
				SelectedPaths.Add(PatrolPath);
				PatrolPath->ToggleDisplay();
			}
		}	

		for (auto& OldPath : this->SelectedPatrolPaths)
		{
			if (IsValid(OldPath.Get()))
			{
				OldPath.Get()->ToggleDisplay();
			}
		}

		this->SelectedPatrolPaths = SelectedPaths;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCrabToolsUE5EditorModule, CrabToolsUE5Editor)