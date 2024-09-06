#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <glm/glm/ext/scalar_constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <physics/boundingBox.hpp>

struct VertexData
{
	std::vector<float> vertices;
	std::vector<float> normals;
};

struct VertexDataIndices
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<float> normals;
};

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
		std::vector<float> cubeVertices = {
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,

			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,

			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f,

			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,

			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f
		};

		return cubeVertices;
	}

	/// <summary>
	/// Returns a vector containing the vertices for a cube arranged in clockwise order (face culling only shows the interior faces, used for skyboxes for example)
	/// </summary>
	/// <returns>A vector of floats</returns>
	static const std::vector<float> getClockwiseCubeVertices()
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
			-1.0f, -1.0f, 0.0f, // Bottom left
			1.0f, 1.0f, 0.0f, // Top right

			1.0f, 1.0f, 0.0f, // Top right
			-1.0f, -1.0f, 0.0f, // Bottom left
			1.0f, -1.0f, 0.0f, // Bottom right
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
			0.0f, 1.0f, // Top left
			0.0f, 0.0f, // Bottom left
			1.0f, 1.0f, // Top right

			1.0f, 1.0f, // Top right
			0.0f, 0.0f, // Bottom left
			1.0f, 0.0f, // Bottom right
		};
		return quadTexCoords;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="slices"></param>
	/// <param name="stacks"></param>
	/// <returns></returns>
	static VertexData getSphereVertices(int slices, int stacks)
	{
		// From https://danielsieger.com/blog/2021/03/27/generating-spheres.html
		std::vector<glm::vec3> vertices;

		// Top vertex
		glm::vec3 v0 = glm::vec3(0.0f, 1.0f, 0.0f);
		vertices.push_back(v0);

		// Generate vertices per stack / slice
		for (int i = 0; i < stacks - 1; i++)
		{
			float phi = glm::pi<float>() * (i + 1) / stacks;

			for (int j = 0; j < slices; j++)
			{
				float theta = 2.0f * glm::pi<float>() * j / slices;
				float x = std::sin(phi) * std::cos(theta);
				float y = std::cos(phi);
				float z = std::sin(phi) * std::sin(theta);
				vertices.push_back(glm::vec3(x, y, z));
			}
		}

		// Bottom vertex
		glm::vec3 v1 = glm::vec3(0.0f, -1.0f, 0.0f);
		vertices.push_back(v1);

		std::vector<glm::vec3> topTriangles;
		std::vector<glm::vec3> bottomTriangles;
		// Add top/bottom triangles
		for (int i = 0; i < slices; i++)
		{
			// Top triangles
			int i0 = i + 1;
			int i1 = (i + 1) % slices + 1;
			topTriangles.push_back(v0);
			topTriangles.push_back(vertices[i1]);
			topTriangles.push_back(vertices[i0]);

			// Bottom triangles
			i0 = i + slices * (stacks - 2) + 1;
			i1 = (i + 1) % slices + slices * (stacks - 2) + 1;
			bottomTriangles.push_back(v1);
			bottomTriangles.push_back(vertices[i0]);
			bottomTriangles.push_back(vertices[i1]);
		}

		std::vector<glm::vec3> quadTriangles;
		// Add quads
		for (int j = 0; j < stacks - 2; j++)
		{
			int j0 = j * slices + 1;
			int j1 = (j + 1) * slices + 1;

			for (int i = 0; i < slices; i++)
			{
				int i0 = j0 + i;
				int i1 = j0 + (i + 1) % slices;
				int i2 = j1 + (i + 1) % slices;
				int i3 = j1 + i;

				quadTriangles.push_back(vertices[i0]);
				quadTriangles.push_back(vertices[i1]);
				quadTriangles.push_back(vertices[i2]);

				quadTriangles.push_back(vertices[i0]);
				quadTriangles.push_back(vertices[i2]);
				quadTriangles.push_back(vertices[i3]);
			}
		}

		VertexData sphereData;

		// Add the vertices for the top triangles
		for (glm::vec3 triangleVert : topTriangles)
		{
			sphereData.vertices.push_back(triangleVert.x);
			sphereData.vertices.push_back(triangleVert.y);
			sphereData.vertices.push_back(triangleVert.z);

			glm::vec3 normal = glm::normalize(glm::vec3(triangleVert.x, triangleVert.y, triangleVert.z));
			sphereData.normals.push_back(normal.x);
			sphereData.normals.push_back(normal.y);
			sphereData.normals.push_back(normal.z);
		}

		// Add the vertices for all the middle quads
		for (glm::vec3 triangleVert : quadTriangles)
		{
			sphereData.vertices.push_back(triangleVert.x);
			sphereData.vertices.push_back(triangleVert.y);
			sphereData.vertices.push_back(triangleVert.z);

			glm::vec3 normal = glm::normalize(glm::vec3(triangleVert.x, triangleVert.y, triangleVert.z));
			sphereData.normals.push_back(normal.x);
			sphereData.normals.push_back(normal.y);
			sphereData.normals.push_back(normal.z);
		}

		// Add the vertices for the bottom triangles
		for (glm::vec3 triangleVert : bottomTriangles)
		{
			sphereData.vertices.push_back(triangleVert.x);
			sphereData.vertices.push_back(triangleVert.y);
			sphereData.vertices.push_back(triangleVert.z);

			glm::vec3 normal = glm::normalize(glm::vec3(triangleVert.x, triangleVert.y, triangleVert.z));
			sphereData.normals.push_back(normal.x);
			sphereData.normals.push_back(normal.y);
			sphereData.normals.push_back(normal.z);
		}

		return sphereData;
	}

	/// <summary>
	/// Optimizes a vector of vertices by using indexed drawing to keep only unique vertices
	/// </summary>
	/// <param name="vertices"></param>
	/// <returns></returns>
	static VertexDataIndices optimizeVertices(std::vector<float> vertices)
	{
		assert(vertices.size() % 9 == 0 && "Vector contains malformed vertice data!");

		// We can't use std::set with a glm::vec3 since it lacks the "<" operator
		// Therefore, we use an unordered set to keep track of what vertices are already present
		// and a vector to keep them in the order of insertion
		std::vector<glm::vec3> uniqueVertices;
		std::unordered_set<glm::vec3> existingVertices;

		std::unordered_map<glm::vec3, unsigned int> vertexToIndex;

		int lastIndex = 0;

		// We start by storing each unique vertex in the vector, and keeping track of it's corresponding index
		for (int i = 0; i < vertices.size(); i += 3)
		{
			glm::vec3 vertex = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			if (existingVertices.count(vertex) == 0)
			{
				existingVertices.insert(vertex);
				uniqueVertices.push_back(vertex);
				vertexToIndex[vertex] = lastIndex++;
			}
		}

		VertexDataIndices vertexData;

		// Second pass to generate the list of indices
		for (int i = 0; i < vertices.size(); i += 3)
		{
			glm::vec3 vertex = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			vertexData.indices.push_back(vertexToIndex[vertex]);
		}

		// Add the vertices to the struct
		for (glm::vec3 uniqueVertex : uniqueVertices)
		{
			vertexData.vertices.push_back(uniqueVertex.x);
			vertexData.vertices.push_back(uniqueVertex.y);
			vertexData.vertices.push_back(uniqueVertex.z);
		}

		return vertexData;
	}

	/// <summary>
	/// Optimizes a vector of vertices by using indexed drawing to keep only unique vertices
	/// </summary>
	/// <param name="vertices"></param>
	/// <returns></returns>
	static VertexDataIndices optimizeVertices(std::vector<float> vertices, std::vector<float> normals)
	{
		assert(vertices.size() % 9 == 0 && "Vector contains malformed vertice data!");

		// We can't use std::set with a glm::vec3 since it lacks the "<" operator
		// Therefore, we use an unordered set to keep track of what vertices are already present
		// and a vector to keep them in the order of insertion
		std::vector<glm::vec3> uniqueVertices;
		std::vector<glm::vec3> uniqueNormals;
		std::unordered_set<glm::vec3> existingVertices;

		std::unordered_map<glm::vec3, unsigned int> vertexToIndex;

		int lastIndex = 0;

		// We start by storing each unique vertex in the vector, and keeping track of it's corresponding index
		for (int i = 0; i < vertices.size(); i += 3)
		{
			glm::vec3 vertex = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			if (existingVertices.count(vertex) == 0)
			{
				glm::vec3 normal = glm::vec3(normals[i], normals[i + 1], normals[i + 2]);

				existingVertices.insert(vertex);
				uniqueVertices.push_back(vertex);
				uniqueNormals.push_back(normal);
				vertexToIndex[vertex] = lastIndex++;
			}
		}

		VertexDataIndices vertexData;

		// Second pass to generate the list of indices
		for (int i = 0; i < vertices.size(); i += 3)
		{
			glm::vec3 vertex = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
			vertexData.indices.push_back(vertexToIndex[vertex]);
		}

		// Add the vertices to the struct
		for (int i = 0; i < uniqueVertices.size(); i++)
		{
			vertexData.vertices.push_back(uniqueVertices[i].x);
			vertexData.vertices.push_back(uniqueVertices[i].y);
			vertexData.vertices.push_back(uniqueVertices[i].z);

			vertexData.normals.push_back(uniqueNormals[i].x);
			vertexData.normals.push_back(uniqueNormals[i].y);
			vertexData.normals.push_back(uniqueNormals[i].z);
		}

		return vertexData;
	}

	/// <summary>
	/// Calculates the per-vertex normals for an array of vertices without indices
	/// </summary>
	/// <param name="vertices">The vertices for which normals should be calculated</param>
	/// <returns>A vector containing the normals for each vertex</returns>
	static std::vector<float> calculateVerticesNormals(std::vector<float> vertices)
	{
		assert(vertices.size() % 9 == 0 && "Vector contains malformed vertice data!");

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
	/// Calculates the per-vertex normals for an array of vertices with indices
	/// </summary>
	/// <param name="vertices">The vertices for which normals should be calculated</param>
	/// <param name="indices"></param>
	/// <returns></returns>
	static std::vector<float> calculateVerticesNormals(std::vector<float> vertices, std::vector<unsigned int> indices)
	{
		assert(indices.size() % 3 == 0 && "Vector contains malformed vertice data!");

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

			// Add the normal to each corresponding vertex as a "weight", which we'll normalize later to keep just the direction
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

	static BoundingBox getMeshBoundingBox(std::vector<float> vertices)
	{
		assert(vertices.size() % 3 == 0 && "Vector contains malformed vertice data!");

		float minX = INFINITY;
		float minY = INFINITY;
		float minZ = INFINITY;

		float maxX = -INFINITY;
		float maxY = -INFINITY;
		float maxZ = -INFINITY;

		for (int i = 0; i < vertices.size(); i += 3)
		{
			if (vertices[i] < minX) minX = vertices[i];
			if (vertices[i + 1] < minY) minY = vertices[i + 1];
			if (vertices[i + 2] < minZ) minZ = vertices[i + 2];

			if (vertices[i] > maxX) maxX = vertices[i];
			if (vertices[i + 1] > maxY) maxY = vertices[i + 1];
			if (vertices[i + 2] > maxZ) maxZ = vertices[i + 2];
		}

		return BoundingBox(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
	}
};
