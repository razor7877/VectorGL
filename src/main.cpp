#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <utilities/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/ext/matrix_transform.hpp>
#include <glm/glm/ext/matrix_clip_space.hpp>
#include <btBulletDynamicsCommon.h>

#include "renderer.hpp"
#include "shader.hpp"
#include "io/input.hpp"
#include "io/interface.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/skyboxComponent.hpp"
#include "components/scriptComponent.hpp"
#include "utilities/resourceLoader.hpp"
#include "logger.hpp"
#include "utilities/geometry.hpp"

extern GLFWwindow* window;

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Creates a renderer for storing & drawing objects
Renderer defaultRenderer = Renderer();
CameraComponent* cameraComponent;

int main()
{
	if (setupGlfwContext() != 0)
		return -1;
	
	// Sets up some parameters for the OpenGL context
	// Depth test for depth buffering, face culling for performance, blending for transparency
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* phongShader = defaultRenderer.shaderManager.getShader(ShaderType::PHONG);
	Shader* pbrShader = defaultRenderer.shaderManager.getShader(ShaderType::PBR);
	Shader* skyboxShader = defaultRenderer.shaderManager.getShader(ShaderType::SKYBOX);

	LightManager::getInstance().shaderProgram = pbrShader;

	constexpr float gravity = -9.81f;

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	btDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0.0f, gravity, 0.0f));

	btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 1.0f);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, -1.0f, 0.0f)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, planeShape, btVector3(0.0f, 0.0f, 0.0f));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

	btCollisionShape* boxShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, 50.0f, 0.0f)));
	btScalar mass = 1.0f;
	btVector3 boxInertia(0.0f, 0.0f, 0.0f);
	boxShape->calculateLocalInertia(mass, boxInertia);
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, boxInertia);
	btRigidBody* boxRigidBody = new btRigidBody(boxRigidBodyCI);
	dynamicsWorld->addRigidBody(boxRigidBody);

	// Simulate the dynamics world
	for (int i = 0; i < 300; i++)
	{
		dynamicsWorld->stepSimulation(1 / 60.f, 10);

		// Print positions of the box
		btTransform trans;
		boxRigidBody->getMotionState()->getWorldTransform(trans);
		std::string output = "Box height: " + std::to_string(trans.getOrigin().getY());
		Logger::logDebug(output);
	}

	// Directional light
	std::unique_ptr<Entity> dirLightEntity = std::unique_ptr<Entity>(new Entity("Directional light"));
	DirectionalLightComponent* directionalLightComponent = dirLightEntity->addComponent<DirectionalLightComponent>();
	defaultRenderer.addEntity(std::move(dirLightEntity));

	// Skybox
	std::unique_ptr<Entity> skyEntity = std::unique_ptr<Entity>(new Entity("Skybox"));
	SkyboxComponent* skyComponent = skyEntity->addComponent<SkyboxComponent>();
	skyComponent->setupSkybox(skyboxShader, defaultRenderer);
	defaultRenderer.addEntity(std::move(skyEntity));

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

		defaultRenderer.addEntity(std::move(pointLightEntity));
	}

	std::unique_ptr<Entity> modelEntity = ResourceLoader::getInstance().loadModelFromFilepath("models/DamagedHelmet.glb", pbrShader);
	ScriptComponent* scriptComponent = modelEntity->addComponent<ScriptComponent>();
	defaultRenderer.addEntity(std::move(modelEntity));

	std::vector<float> quadVertices = Geometry::getQuadVertices();
	std::unique_ptr<Entity> planeEntity = std::make_unique<Entity>("Plane");
	MeshComponent* planeMesh = planeEntity->addComponent<MeshComponent>();
	planeMesh->setupMesh(&quadVertices[0], quadVertices.size() * sizeof(float), pbrShader);
	planeEntity->transform->setPosition(0.0f, -5.0f, 0.0f);
	planeEntity->transform->setRotation(-90.0f, 0.0f, 0.0f);
	planeEntity->transform->setScale(glm::vec3(20.0f));
	defaultRenderer.addEntity(std::move(planeEntity));

	// After all needed objects have been added, initializes the renderer's data to set up every object's data
	defaultRenderer.init(glm::vec2(windowWidth, windowHeight));
	cameraComponent = defaultRenderer.currentCamera;

	//Texture* hdrMap = new Texture("img/fouriesburg_mountain_lookout_2_8k.hdr", TextureType::TEXTURE_DIFFUSE, true, true);
	//IBLData skyIBL = IBLData(defaultRenderer, hdrMap);

	// Initializes the ImGui UI system
	ImGuiInit(window, &defaultRenderer);
	
	// A simple variable to retrieve the current glGetError() code and decide whether to print it to console
	int glErrorCurrent;
	// A variable that stores the current frame's timestamp, to calculate time between frames
	float currentFrame;

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Clears the buffers and last frame before rendering the next one
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculates elapsed time since last frame for time-based calculations
		currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Processes any mouse or keyboard input for camera movement
		processInput(window, deltaTime);
		
		phongShader->use()
			->setVec3("viewPos", cameraComponent->getPosition());

		pbrShader->use()
			->setVec3("camPos", cameraComponent->getPosition());

		defaultRenderer.render(deltaTime);

		/*std::unique_ptr<Entity> newEntity = ResourceLoader::getInstance().loadModelFromFilepath("models/DamagedHelmet.glb", LightManager::getInstance().shaderProgram);
		if (newEntity != nullptr)
		{
			newEntity->start();
		}*/
		
		// Draws the ImGui interface windows
		ImGuiDrawWindows();

		// Print error code to console if there is one
		glErrorCurrent = glGetError();
		if (glErrorCurrent != 0) { Logger::logError(std::string("OpenGL error code: ") + std::to_string(glErrorCurrent)); }

		// Swaps buffers to screen to show the rendered frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
