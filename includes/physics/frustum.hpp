#pragma once

#include "physics/plane.hpp"

/// <summary>
/// Represents a frustum
/// </summary>
struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};
