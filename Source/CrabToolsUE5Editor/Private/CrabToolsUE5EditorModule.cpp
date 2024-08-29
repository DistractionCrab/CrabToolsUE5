#include "CrabToolsUE5EditorModule.h"
#include "Selection.h"
#include "Actors/PatrolPath.h"

#define LOCTEXT_NAMESPACE "FCrabToolsUE5EditorModule"

const FName CrabToolsUE5EditorAppIdentifier = FName(TEXT("CrabToolsUE5EditorApp"));


FCrabToolsUE5EditorModule::FCrabToolsUE5EditorModule() {

}

void FCrabToolsUE5EditorModule::StartupModule() 
{
	this->SelectionChangedHandle = USelection::SelectionChangedEvent.AddRaw(
		this,
		&FCrabToolsUE5EditorModule::OnSelectionChanged);
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