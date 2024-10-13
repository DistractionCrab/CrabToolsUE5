#pragma once


namespace AI_Events
{
	// Event for when a path following entity has arrived at their desitnation.
	constexpr char AI_ARRIVE[] = "AI_ON_ARRIVE";

	// Event for when a path following entity cannot reach their desitnation for some reason.
	constexpr char AI_LOST[] = "AI_ON_LOST";

	// Event for when a wait has finished.
	constexpr char AI_WAIT_FINISHED[] = "AI_ON_WAIT_FINISHED";

	// Event for when an ability has finished.
	constexpr char AI_ABILITY_FINISHED[] = "AI_ABILITY_FINISHED";

	// Event for when a target cannot be interacted with.
	constexpr char AI_CANNOT_INTERACT[] = "AI_CANNOT_INTERACT";
}