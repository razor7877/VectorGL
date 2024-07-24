#pragma once

#include <vector>

/// <summary>
/// A utility class that provides methods for creating geometric primitives, getting texture coordinates, normals etc.
/// </summary>
class Geometry
{
public:
	/// <summary>
	/// Returns a vector containing the vertices for a cube
	/// </summary>
	/// <returns>A vector of floats</returns>
	static const std::vector<float> getCubeVertices()
	{
		std::vector<float> cubeVertices = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };
		return cubeVertices;
	}

	/// <summary>
	/// Returns a vector containing the vertices for a quad
	/// </summary>
	/// <returns>A vector of floats</returns>
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

	/// <summary>
	/// Returns a vector containing the texture coordinates for a quad
	/// </summary>
	/// <returns>A vector of floats</returns>
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

	/// <summary>
	/// Calculates the per-vertex normals for an array of vertices without indices
	/// </summary>
	/// <param name="vertices">The vertices for which normals should be calculated</param>
	/// <returns>A vector containing the normals for each vertex</returns>
	static std::vector<float> calculateVerticesNormals(std::vector<float> vertices)
	{
		std::vector<float> normals;

		// Iterates over the vector triangle by triangle
		for (int i = 0; i < vertices.size(); i += 9)
		{
			glm::vec3 v1 = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			glm::vec3 v2 = glm::vec3(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
			glm::vec3 v3 = glm::vec3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

			// We calculate two edges so we can get the cross product
			glm::vec3 edge1 = v2 - v1;
			glm::vec3 edge2 = v3 - v1;

			// Calculate the normal
			glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

			// Each vertice is given the face normal as normal
			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);

			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);

			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);
		}

		return normals;
	}

	/// <summary>
	/// Calculates the per-vertex normals for an array of vertices without indices
	/// </summary>
	/// <param name="vertices"></param>
	/// <param name="indices"></param>
	/// <returns></returns>
	static std::vector<float> calculateVerticesNormals(std::vector<float> vertices, std::vector<unsigned int> indices)
	{
		assert(indices.size() % 3 == 0, "Vector contains malformed vertice data!");

		std::vector<glm::vec3> normals(vertices.size() / 3, glm::vec3(0.0f));

		for (int i = 0; i < indices.size(); i += 3)
		{
			unsigned int i1 = indices[i];
			unsigned int i2 = indices[i + 1];
			unsigned int i3 = indices[i + 2];

			glm::vec3 v1 = glm::vec3(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
			glm::vec3 v2 = glm::vec3(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);
			glm::vec3 v3 = glm::vec3(vertices[i3 * 3], vertices[i3 * 3 + 1], vertices[i3 * 3 + 2]);

			// We calculate two edges so we can get the cross product
			glm::vec3 edge1 = v2 - v1;
			glm::vec3 edge2 = v3 - v1;

			// Calculate the normal
			glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

			// Add the normal to each corresponding vertex as a "weight"
			normals[i1] += normal;
			normals[i2] += normal;
			normals[i3] += normal;
		}

		std::vector<float> normalsAsFloats;

		// Normalize all normals and them to the vector as floats
		for (int i = 0; i < normals.size(); i++)
		{
			glm::vec3 normalized = glm::normalize(normals[i]);
			normalsAsFloats.push_back(normalized.x);
			normalsAsFloats.push_back(normalized.y);
			normalsAsFloats.push_back(normalized.z);
		}

		return normalsAsFloats;
	}
};