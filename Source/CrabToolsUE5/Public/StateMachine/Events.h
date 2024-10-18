#pragma once


namespace Events::AI
{
	// Event for when a path following entity has arrived at their desitnation.
	constexpr char ARRIVE[] = "AI_ON_ARRIVE";

	// Event for when a path following entity cannot reach their desitnation for some reason.
	constexpr char LOST[] = "AI_ON_LOST";

	// Event for when a wait has finished.
	constexpr char WAIT_FINISHED[] = "AI_ON_WAIT_FINISHED";

	// Event for when an ability has finished.
	constexpr char ABILITY_FINISHED[] = "AI_ABILITY_FINISHED";

	// Event for when a target cannot be interacted with.
	constexpr char CANNOT_INTERACT[] = "AI_CANNOT_INTERACT";

	// Event for when data is passed that doesn't implement that TargetingControllerInterface.
	constexpr char INVALID_TARGETING[] = "AI_INVALID_TARGETING";

	// Event for when targets have been confirmed by the targeting controller.
	constexpr char TARGETS_CONFIRMED[] = "AI_CONFIRMED_TARGETS";
}

namespace Events::Animation
{
	// Event for when a path following entity has arrived at their desitnation.
	constexpr char NOTIFY_FINISH_WAIT[] = "ANIM_NOTIFY_FINISH_WAIT";
}

namespace Events::Tasks
{
	// Event for when a task has been completed.
	constexpr char COMPLETED[] = "TASK_COMPLETED";

	// Event emitted when no task has been received.
	constexpr char NO_TASK[] = "TASK_NULL";
}