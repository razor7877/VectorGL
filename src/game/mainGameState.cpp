#include <memory>
#include <vector>

#include "game/mainGameState.hpp"
#include "io/input.hpp"
#include "shader.hpp"
#include "entity.hpp"
#include "renderer.hpp"
#include "lightManager.hpp"
#include "components/meshComponent.hpp"
#include "components/skyboxComponent.hpp"
#include "components/physicsComponent.hpp"
#include "components/cameraComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "utilities/geometry.hpp"
#include "materials/pbrMaterial.hpp"
#include "main.hpp"
#include "logger.hpp"

void MainGameState::init()
{
	Shader* phongShader = this->renderer.shaderManager.getShader(ShaderType::PHONG);
	Shader* pbrShader = this->renderer.shaderManager.getShader(ShaderType::PBR);
	Shader* skyboxShader = this->renderer.shaderManager.getShader(ShaderType::SKYBOX);

	LightManager::getInstance().shaderProgram = pbrShader;

	VertexData sphere = Geometry::getSphereVertices(100, 30);
	VertexDataIndices sphereOptimized = Geometry::optimizeVertices(sphere.vertices, sphere.normals);

	// Create the camera and set it up
	std::unique_ptr<Entity> cameraEntity = std::unique_ptr<Entity>(new Entity("Camera"));
	MeshComponent* cameraMesh = cameraEntity->addComponent<MeshComponent>();
	
	cameraMesh->setMaterial(std::make_unique<PBRMaterial>(game.renderer.shaderManager.getShader(ShaderType::PBR)))
		.addVertices(sphereOptimized.vertices)
		.addIndices(sphereOptimized.indices)
		.addNormals(sphereOptimized.normals)
		.setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
	this->scene.currentCamera = cameraEntity->addComponent<CameraComponent>();
	this->scene.addEntity(std::move(cameraEntity));

	std::unique_ptr<Entity> skyCameraEntity = std::unique_ptr<Entity>(new Entity("Sky Camera"));
	this->scene.skyCamera = skyCameraEntity->addComponent<CameraComponent>();
	this->scene.skyCamera->setPosition(glm::vec3(0.0f, 75.0f, 0.0f));
	this->scene.skyCamera->setZoom(90.0f);
	skyCameraEntity->transform->setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
	this->scene.addEntity(std::move(skyCameraEntity));

	LightManager::getInstance().init();

	// Directional light
	std::unique_ptr<Entity> dirLightEntity = std::unique_ptr<Entity>(new Entity("Directional light"));
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	this->scene.directionalLight = directionalLightComponent;
	this->scene.addEntity(std::move(dirLightEntity));

	// Cube
	std::unique_ptr<Entity> cubeEntity = std::make_unique<Entity>("Cube");

	MeshComponent* cubeMesh = cubeEntity->addComponent<MeshComponent>();
	std::vector<float> cubeVertices = Geometry::getCubeVertices();
	cubeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(cubeVertices);

	PhysicsComponent* cubeCollider = cubeEntity->addComponent<PhysicsComponent>();
	this->physicsWorld.addBox(cubeCollider, glm::vec3(1.0f), glm::vec3(0.0f));

	this->scene.addEntity(std::move(cubeEntity));

	cubeEntity = std::make_unique<Entity>("Cube");

	cubeMesh = cubeEntity->addComponent<MeshComponent>();
	cubeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(cubeVertices);

	cubeCollider = cubeEntity->addComponent<PhysicsComponent>();
	this->physicsWorld.addBox(cubeCollider, glm::vec3(1.0f), glm::vec3(0.0f));

	this->scene.addEntity(std::move(cubeEntity));

	// Sphere grid
	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			for (int z = 0; z < 13; z++)
			{
				std::unique_ptr<Entity> sphereEntity = std::make_unique<Entity>("Sphere");

				MeshComponent* sphereMesh = sphereEntity->addComponent<MeshComponent>();
				sphereMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
					.addVertices(sphereOptimized.vertices)
					.addIndices(sphereOptimized.indices)
					.addNormals(sphereOptimized.normals);

				sphereMesh->setDiffuseColor(glm::vec3((float)x / 13.0f, (float)y / 13.0f, 1.0f));
				sphereEntity->transform->setPosition(x * 3, y * 3, z * 3);

				PBRMaterial* pbrMat = dynamic_cast<PBRMaterial*>(sphereMesh->material.get());
				if (pbrMat != nullptr)
				{
					pbrMat->roughness = (float)y / 13.0f + 0.001f;
					pbrMat->metallic = (float)z / 13.0f;
				}

				this->scene.addEntity(std::move(sphereEntity));
			}
		}
	}

	// Skybox
	std::unique_ptr<Entity> skyEntity = std::unique_ptr<Entity>(new Entity("Skybox"));
	SkyboxComponent* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader, this->renderer);
	this->scene.addEntity(std::move(skyEntity));

	// Lights
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	for (int i = 0; i < 4; i++)
	{
		// Add point light
		std::unique_ptr<Entity> pointLightEntity = std::unique_ptr<Entity>(new Entity("Point light"));
		pointLightEntity->transform->setScale(glm::vec3(0.1f));
		pointLightEntity->transform->setPosition(lightPositions[i]);
		PointLightComponent* pointLightComponent = pointLightEntity->addComponent<PointLightComponent>();
		pointLightComponent->diffuseColor = lightColors[i];

		this->scene.addEntity(std::move(pointLightEntity));
	}

	// Plane
	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::unique_ptr<Entity> planeEntity = std::make_unique<Entity>("Plane");

	MeshComponent* planeMesh = planeEntity->addComponent<MeshComponent>();
	planeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(quadVertices);
	planeEntity->transform->setPosition(0.0f, -5.0f, 0.0f);
	planeEntity->transform->setRotation(-90.0f, 0.0f, 0.0f);
	planeEntity->transform->setScale(glm::vec3(20.0f, 20.0f, 1.0f));

	this->scene.addEntity(std::move(planeEntity));
	this->physicsWorld.addPlane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f));

	// Initialize scene
	this->scene.init();
}

void MainGameState::cleanup()
{
	this->scene.end();
}

void MainGameState::pause()
{

}

void MainGameState::resume()
{

}

// TODO : Find a better way of handling inputs
extern GLFWwindow* window;

void MainGameState::handleEvents(GameEngine* gameEngine, float deltaTime)
{
	CameraComponent* camera = this->scene.currentCamera;
	auto lambda = [camera, this, gameEngine](GLFWwindow* window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Forward movement
			camera->processKeyboard(CameraMovement::FORWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Backward movement
			camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Left movement
			camera->processKeyboard(CameraMovement::LEFT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Right movement
			camera->processKeyboard(CameraMovement::RIGHT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
			gameEngine->popState();
	};

	// Processes any mouse or keyboard input for camera movement
	processInput(lambda, window, deltaTime);
}

void MainGameState::update(GameEngine* gameEngine, float deltaTime)
{
	this->renderer.render(this->scene, this->physicsWorld, deltaTime);
}

void MainGameState::draw(GameEngine* gameEngine)
{

}
