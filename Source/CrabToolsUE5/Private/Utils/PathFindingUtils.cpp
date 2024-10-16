#include "Utils/PathFindingUtils.h"
#include "NavigationSystemTypes.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AITypes.h"

FVector UPathFindingUtilsLibrary::ChooseNearLocation(AAIController* Ctrl, TArray<FVector>& Points)
{
	UNavigationSystemV1* NavSys = Ctrl ? FNavigationSystem::GetCurrent<UNavigationSystemV1>(Ctrl->GetWorld()) : nullptr;

	if (Ctrl)
	{
		const auto AgentLocation = Ctrl->GetNavAgentLocation();
		const ANavigationData* NavData = NavSys->GetNavDataForProps(Ctrl->GetNavAgentPropertiesRef(), AgentLocation);
		
		FVector Dest;
		bool bFoundPath = false;
		float PathLength = std::numeric_limits<float>::infinity();

		for (auto& Pt : Points)
		{
			FPathFindingQuery Query(Ctrl, *NavData, AgentLocation, Pt);
			FNavPathSharedPtr NavPath;

			auto Result = NavSys->FindPathSync(Query);

			if (Result.IsSuccessful())
			{
				bFoundPath = true;
				float Length = Result.Path->GetLength();

				if (Length < PathLength)
				{
					Dest = Pt;
					PathLength = Length;
				}
			}
		}

		if (bFoundPath) { return Dest; }
	}

	if (Points.Num() > 0)
	{
		return Points[0];
	}
	else
	{
		return FVector::Zero();
	}
}