#include <memory>

#include <utilities/glad.h>
#include <glfw/glfw3.h>

#include "game/startMenuState.hpp"
#include "game/mainGameState.hpp"
#include "io/input.hpp"
#include "utilities/geometry.hpp"
#include "main.hpp"
#include "components/skyboxComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/lights/pointLightComponent.hpp"

void StartMenuState::init()
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

	LightManager::getInstance().init();

	// Directional light
	std::unique_ptr<Entity> dirLightEntity = std::unique_ptr<Entity>(new Entity("Directional light"));
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	this->scene.directionalLight = directionalLightComponent;
	this->scene.addEntity(std::move(dirLightEntity));

	// Sphere
	for (int i = 0; i < 10; i++)
	{
		std::unique_ptr<Entity> sphereEntity = std::make_unique<Entity>("Sphere");

		MeshComponent* sphereMesh = sphereEntity->addComponent<MeshComponent>();
		sphereMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
			.addVertices(sphereOptimized.vertices)
			.addIndices(sphereOptimized.indices)
			.addNormals(sphereOptimized.normals);

		PhysicsComponent* sphereCollider = sphereEntity->addComponent<PhysicsComponent>();
		this->physicsWorld.addSphere(sphereCollider, 1.0f, glm::vec3(0.0f, 25.0f, 0.0f));

		this->scene.addEntity(std::move(sphereEntity));
	}

	// Skybox
	std::unique_ptr<Entity> skyEntity = std::unique_ptr<Entity>(new Entity("Skybox"));
	SkyboxComponent* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader, this->renderer);
	skyComponent->changeSkybox(SkyboxType::NIGHT);
	this->scene.addEntity(std::move(skyEntity));

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

void StartMenuState::cleanup()
{
	this->renderer.end();
}

void StartMenuState::pause()
{

}

void StartMenuState::resume()
{

}

extern GLFWwindow* window;

void StartMenuState::handleEvents(GameEngine* gameEngine, float deltaTime)
{
	CameraComponent* camera = this->scene.currentCamera;
	auto lambda = [camera, this, gameEngine](GLFWwindow* window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
			gameEngine->pushState(new MainGameState(this->renderer));

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Forward movement
			camera->processKeyboard(CameraMovement::FORWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Backward movement
			camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Left movement
			camera->processKeyboard(CameraMovement::LEFT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Right movement
			camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
	};

	processInput(lambda, window, deltaTime);
}

void StartMenuState::update(GameEngine* gameEngine, float deltaTime)
{
	this->renderer.render(this->scene, this->physicsWorld, deltaTime);
}

void StartMenuState::draw(GameEngine* gameEngine)
{

}