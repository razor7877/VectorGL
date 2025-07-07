#include <memory>

#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "game/StartMenuState.hpp"
#include "game/MainGameState.hpp"
#include "io/input.hpp"
#include "utilities/geometry.hpp"
#include "main.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/lights/DirectionalLightComponent.hpp"
#include "Logger.hpp"
#include "LightManager.hpp"
#include "materials/PBRMaterial.hpp"

void StartMenuState::init()
{
	Shader* pbrShader = this->renderer.shaderManager.getShader(ShaderType::PBR);
	Shader* skyboxShader = this->renderer.shaderManager.getShader(ShaderType::SKYBOX);

	LightManager::getInstance().shaderProgram = pbrShader;

	VertexData sphere = Geometry::getSphereVertices(100, 30);
	VertexDataIndices sphereOptimized = Geometry::optimizeVertices(sphere.vertices, sphere.normals);

	// Create the camera and set it up
	std::unique_ptr<Entity> cameraEntity = std::make_unique<Entity>("Camera");
	cameraEntity->isStatic = false;
	auto* cameraMesh = cameraEntity->addComponent<MeshComponent>();

	cameraMesh->setMaterial(std::make_unique<PBRMaterial>(Main::game.renderer.shaderManager.getShader(ShaderType::PBR)))
		.addVertices(sphereOptimized.vertices)
		.addIndices(sphereOptimized.indices)
		.addNormals(sphereOptimized.normals)
		.setDiffuseColor(glm::vec3(1.0f, 0.0f, 0.0f));
	this->scene.currentCamera = cameraEntity->addComponent<CameraComponent>();
	this->scene.addEntity(std::move(cameraEntity));

	cameraEntity = std::make_unique<Entity>("Sky Camera");
	this->scene.skyCamera = cameraEntity->addComponent<CameraComponent>();
	cameraEntity->getTransform()->setPosition(0.0f, 20.0f, 0.0f);
	cameraEntity->getTransform()->setRotation(0.0f, -90.0f, 0.0f);
	this->scene.addEntity(std::move(cameraEntity));

	LightManager::getInstance().init();

	// Directional light
	std::unique_ptr<Entity> dirLightEntity = std::make_unique<Entity>("Directional light");
	auto* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	this->scene.directionalLight = directionalLightComponent;
	this->scene.addEntity(std::move(dirLightEntity));

	// Sphere
	for (int i = 0; i < 10; i++)
	{
		std::unique_ptr<Entity> sphereEntity = std::make_unique<Entity>("Sphere");
		sphereEntity->isStatic = false;

		auto* sphereMesh = sphereEntity->addComponent<MeshComponent>();
		sphereMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
			.addVertices(sphereOptimized.vertices)
			.addIndices(sphereOptimized.indices)
			.addNormals(sphereOptimized.normals);

		auto* sphereCollider = sphereEntity->addComponent<PhysicsComponent>();
		this->physicsWorld.addSphere(sphereCollider, 1.0f, glm::vec3(0.0f, 25.0f, 0.0f));

		this->scene.addEntity(std::move(sphereEntity));
	}

	// Sphere grid
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			for (int z = 0; z < 5; z++)
			{
				std::unique_ptr<Entity> sphereEntity = std::make_unique<Entity>("Sphere");

				auto* sphereMesh = sphereEntity->addComponent<MeshComponent>();
				sphereMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
					.addVertices(sphereOptimized.vertices)
					.addIndices(sphereOptimized.indices)
					.addNormals(sphereOptimized.normals);

				sphereMesh->setDiffuseColor(glm::vec3(static_cast<float>(x) / 13.0f, static_cast<float>(y) / 13.0f, 1.0f));
				sphereEntity->getTransform()->setPosition(x * 3, y * 3, z * 3);

				auto* pbrMat = dynamic_cast<PBRMaterial*>(sphereMesh->material.get());
				if (pbrMat != nullptr)
				{
					pbrMat->roughness = static_cast<float>(y) / 13.0f + 0.001f;
					pbrMat->metallic = static_cast<float>(z) / 13.0f;
					pbrMat->opacity = 0.5f;
				}

				this->scene.addEntity(std::move(sphereEntity));
			}
		}
	}

	// Skybox
	std::unique_ptr<Entity> skyEntity = std::make_unique<Entity>("Skybox");
	auto* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader, this->renderer);
	skyComponent->changeSkybox(SkyboxType::SKY);
	this->scene.addEntity(std::move(skyEntity));

	// Plane
	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::unique_ptr<Entity> planeEntity = std::make_unique<Entity>("Plane");

	auto* planeMesh = planeEntity->addComponent<MeshComponent>();
	planeMesh->setMaterial(std::make_unique<PBRMaterial>(pbrShader))
		.addVertices(quadVertices);
	planeEntity->getTransform()->setPosition(0.0f, -5.0f, 0.0f);
	planeEntity->getTransform()->setRotation(-90.0f, 0.0f, 0.0f);
	planeEntity->getTransform()->setScale(glm::vec3(20.0f, 20.0f, 1.0f));

	this->scene.addEntity(std::move(planeEntity));

	this->physicsWorld.addPlane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f));

	// Initialize scene
	this->scene.init();
}

void StartMenuState::cleanup()
{
	this->scene.end();
}

void StartMenuState::pause()
{

}

void StartMenuState::resume()
{

}

void StartMenuState::handleEvents(GameEngine* gameEngine, float deltaTime)
{
	if (Input::isKeyPressed(GLFW_KEY_ENTER))
	{
		Logger::logDebug("Pushed state at frame " + std::to_string(Main::frameCounter), "startMenuState.cpp");
		gameEngine->pushState(std::make_unique<MainGameState>(this->renderer));
		return;
	}

	CameraComponent* camera = this->scene.currentCamera;

	if (Input::isKeyHeld(GLFW_KEY_W)) // Forward movement
		camera->processKeyboard(CameraMovement::FORWARD, deltaTime);

	if (Input::isKeyHeld(GLFW_KEY_S)) // Backward movement
		camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);

	if (Input::isKeyHeld(GLFW_KEY_A)) // Left movement
		camera->processKeyboard(CameraMovement::LEFT, deltaTime);

	if (Input::isKeyHeld(GLFW_KEY_D)) // Right movement
		camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
}

void StartMenuState::update(GameEngine* gameEngine, float deltaTime)
{
	
}

void StartMenuState::draw(GameEngine* gameEngine, float deltaTime)
{
	this->renderer.render(this->scene, this->physicsWorld, deltaTime);
}
