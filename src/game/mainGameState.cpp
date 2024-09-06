#include <memory>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <utilities/glad.h>
#include <GLFW/glfw3.h>

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
	
	cameraMesh->setMaterial(std::make_unique<PBRMaterial>(Main::game.renderer.shaderManager.getShader(ShaderType::PBR)))
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
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			for (int z = 0; z < 5; z++)
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
					pbrMat->opacity = 0.5f;
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

	ghostObject = new btPairCachingGhostObject();
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0, 10, 0)); // Start position

	this->characterShape = new btCapsuleShape(0.5f, 2.0f); // radius, height

	ghostObject->setWorldTransform(startTransform);
	ghostObject->setCollisionShape(characterShape);
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	this->characterController = new btKinematicCharacterController(ghostObject, characterShape, 0.35f);
	this->characterController->setGravity(btVector3(0.0f, -9.81f, 0.0f));

	this->physicsWorld.world->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	this->physicsWorld.world->addAction(characterController);

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

void MainGameState::handleEvents(GameEngine* gameEngine, float deltaTime)
{
	CameraComponent* camera = this->scene.currentCamera;
	auto lambda = [camera, this, gameEngine](GLFWwindow* window, float deltaTime)
	{
		// Z forward vector
		glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, 1.0f);
		// The camera forward vector
		glm::vec3 cameraFront = this->getScene().currentCamera->getForward();
		// The camera forward projected onto the XZ plane, we don't have Y rotation
		glm::vec3 cameraFrontXz = glm::vec3(cameraFront.x, 0.0f, cameraFront.z);

		float dot = glm::dot(worldFront, cameraFrontXz);
		// The angle between the world and camera forward
		float angle = acos(dot);

		// We need to check if the angle is a clockwise or counter clockwise rotation
		glm::vec3 cross = glm::cross(worldFront, cameraFrontXz);
		if (glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), cameraFront) > 0.0f)
			angle = -angle;

		Logger::logDebug(std::to_string(angle), "mainGameState.cpp");

		btVector3 walkDirection(0.0f, 0.0f, 0.0f);
		btScalar walkSpeed = 5.0f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Forward movement
			walkDirection += btVector3(0.0f, 0.0f, 1.0f);

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Backward movement
			walkDirection += btVector3(0.0f, 0.0f, -1.0f);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Left movement
			walkDirection += btVector3(1.0f, 0.0f, 0.0f);

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Right movement
			walkDirection += btVector3(-1.0f, 0.0f, 0.0f);

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) // Quit to previous state
			gameEngine->popState();

		// We need to rotate the walk direction using the camera forward for the movement to be relative to it

		//walkDirection = btVector3(walkDirection.x() * cos(angle), 0.0f, walkDirection.z() * sin(angle));

		btScalar newX = walkDirection.x() * cos(angle) - walkDirection.z() * sin(angle);
		btScalar newZ = walkDirection.z() * cos(angle) + walkDirection.x() * sin(angle);

		walkDirection = btVector3(newX, 0.0f, newZ);

		// Normalize the direction to avoid diagonal movement being faster
		if (!walkDirection.fuzzyZero())
			walkDirection = walkDirection.normalize() * walkSpeed * deltaTime;

		this->characterController->setWalkDirection(walkDirection);

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (this->characterController->onGround())
				this->characterController->jump(btVector3(0.0f, 10.0f, 0.0f));
		}
	};

	// Processes any mouse or keyboard input for camera movement
	Input::processInput(lambda, Input::inputData.window, deltaTime);
}

void MainGameState::update(GameEngine* gameEngine, float deltaTime)
{
	btPairCachingGhostObject* ghostObject = this->characterController->getGhostObject();
	glm::mat4 transform;
	ghostObject->getWorldTransform().getOpenGLMatrix(glm::value_ptr(transform));
	this->scene.currentCamera->setPosition(glm::vec3(transform[3][0], transform[3][1], transform[3][2]));

	this->renderer.render(this->scene, this->physicsWorld, deltaTime);
}

void MainGameState::draw(GameEngine* gameEngine)
{

}
