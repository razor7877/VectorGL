#include <stdexcept>

#include "render/DebugRenderPass.hpp"
#include "Renderer.hpp"

DebugRenderPass::DebugRenderPass() : RenderPass()
{

}

DebugRenderPass::~DebugRenderPass()
{
    this->renderTarget.reset();
}

void DebugRenderPass::execute(Renderer& renderer, const Scene& scene, float deltaTime)
{
	throw std::runtime_error("DebugRenderPass::execute() should not be called without a render target!");
}

void DebugRenderPass::execute(RenderTarget& outTarget, Renderer& renderer, const Scene& scene, float deltaTime)
{
	outTarget.bind();

	// Debug bounding boxes
	for (MeshComponent* mesh : scene.sortedSceneData.meshes)
	{
		std::vector<float> vertices;

		// Querying the bounding boxes like that every frame is super slow, could probably be improved
		BoundingBox meshBB = mesh->getWorldBoundingBox();
		glm::vec3 minPos = meshBB.minPosition;
		glm::vec3 maxPos = meshBB.maxPosition;

		// Create the vertices for the 8 points of the bounding box
		// Left bottom back
		vertices.push_back(minPos[0]); vertices.push_back(minPos[1]); vertices.push_back(minPos[2]); // (x_min, y_min, z_min)
		// Left bottom front
		vertices.push_back(minPos[0]); vertices.push_back(minPos[1]); vertices.push_back(maxPos[2]); // (x_min, y_min, z_max)
		// Left top back
		vertices.push_back(minPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(minPos[2]); // (x_min, y_max, z_min)
		// Left top front
		vertices.push_back(minPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(maxPos[2]); // (x_min, y_max, z_max)
		// Right bottom back
		vertices.push_back(maxPos[0]); vertices.push_back(minPos[1]); vertices.push_back(minPos[2]); // (x_max, y_min, z_min)
		// Right bottom front
		vertices.push_back(maxPos[0]); vertices.push_back(minPos[1]); vertices.push_back(maxPos[2]); // (x_max, y_min, z_max)
		// Right top back
		vertices.push_back(maxPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(minPos[2]); // (x_max, y_max, z_min)
		// Right top front
		vertices.push_back(maxPos[0]); vertices.push_back(maxPos[1]); vertices.push_back(maxPos[2]); // (x_max, y_max, z_max)

		// The indices for creating lines that links all the points of the bounding box using the 8 previous vertices

		// Add the vertices to draw each line of the bounding box
		for (int i = 0; i < 12; ++i)
		{
			int edgeIndices[12][2] = {
				{0, 1}, {0, 2}, {1, 3}, {2, 3}, // Left side edges
				{4, 5}, {4, 6}, {5, 7}, {6, 7}, // Right side edges
				{0, 4}, {2, 6}, {1, 5}, {3, 7}, // Connect the two sides
			};

			int v1 = edgeIndices[i][0];
			int v2 = edgeIndices[i][1];

			// Add the coordinates of the two vertices for each edge
			debugLines.push_back(vertices[v1 * 3 + 0]); // x of v1
			debugLines.push_back(vertices[v1 * 3 + 1]); // y of v1
			debugLines.push_back(vertices[v1 * 3 + 2]); // z of v1

			debugLines.push_back(vertices[v2 * 3 + 0]); // x of v2
			debugLines.push_back(vertices[v2 * 3 + 1]); // y of v2
			debugLines.push_back(vertices[v2 * 3 + 2]); // z of v2
		}
	}

	// Line drawing for debugging raycasts etc.
	std::vector<float> linesToDraw = this->storedDebugLines;
	linesToDraw.insert(linesToDraw.end(), this->debugLines.begin(), this->debugLines.end());

	if (!linesToDraw.empty())
	{
		GLuint lineVAO;
		GLuint lineVBO;

		glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, debugLines.size() * sizeof(float), &debugLines[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		renderer.shaderManager.getShader(ShaderType::SOLID)->use();
		glBindVertexArray(lineVAO);
		glLineWidth(25.0f);
		glDrawArrays(GL_LINES, 0, debugLines.size() / 3);
		debugLines.clear();

		glDeleteVertexArrays(1, &lineVAO);
		glDeleteBuffers(1, &lineVBO);
	}

	outTarget.unbind();
}

void DebugRenderPass::addLine(glm::vec3 startPos, glm::vec3 endPos, bool store)
{
	if (store)
	{
		this->storedDebugLines.push_back(startPos.x);
		this->storedDebugLines.push_back(startPos.y);
		this->storedDebugLines.push_back(startPos.z);
		this->storedDebugLines.push_back(endPos.x);
		this->storedDebugLines.push_back(endPos.y);
		this->storedDebugLines.push_back(endPos.z);
	}

	this->debugLines.push_back(startPos.x);
	this->debugLines.push_back(startPos.y);
	this->debugLines.push_back(startPos.z);
	this->debugLines.push_back(endPos.x);
	this->debugLines.push_back(endPos.y);
	this->debugLines.push_back(endPos.z);
}


void DebugRenderPass::addLines(const std::vector<float>& lines, bool store)
{
	if (store)
		this->storedDebugLines.insert(this->storedDebugLines.end(), lines.begin(), lines.end());
	else
		this->debugLines.insert(this->debugLines.end(), lines.begin(), lines.end());
}
