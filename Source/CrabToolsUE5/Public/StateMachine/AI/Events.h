#pragma once


namespace AI_Events
{
	// Event for when a path following entity has arrived at their desitnation.
	constexpr char AI_ARRIVE[] = "AI_ON_ARRIVE";
	// Event for when a path following entity cannot reach their desitnation for some reason.
	constexpr char AI_LOST[] = "AI_ON_LOST";
}