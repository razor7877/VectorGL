#pragma once

#include <vector>

class Geometry
{
public:
	static const std::vector<float> getCubeVertices()
	{
		std::vector<float> cubeVertices = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };
		return cubeVertices;
	}

	static const std::vector<float> getQuadVertices()
	{
		std::vector<float> quadVertices = {
			-1.0f, 1.0f, 0.0f, // Top left
			1.0f, 1.0f, 0.0f, // Top right
			-1.0f, -1.0f, 0.0f, // Bottom left

			1.0f, 1.0f, 0.0f, // Top right
			1.0f, -1.0f, 0.0f, // Bottom right
			-1.0f, -1.0f, 0.0f, // Bottom left
		};
		return quadVertices;
	}

	static const std::vector<float> getQuadTexCoords()
	{
		std::vector<float> quadTexCoords = {
			0.0f, 0.0f, // Top left
			1.0f, 0.0f, // Top right
			0.0f, 1.0f, // Bottom left

			1.0f, 0.0f, // Top right
			1.0f, 1.0f, // Bottom right
			0.0f, 1.0f, // Bottom left
		};
		return quadTexCoords;
	}
};