#include "physics/physicsWorld.hpp"

PhysicsWorld::PhysicsWorld()
{
	this->collisionConfiguration = new btDefaultCollisionConfiguration();
	this->collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
	this->overlappingPairCache = new btDbvtBroadphase();
	btOverlappingPairCallback* m_ghostPairCallback = new btGhostPairCallback();
	this->overlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(m_ghostPairCallback);
	this->solver = new btSequentialImpulseConstraintSolver();
	this->world = new btDiscreteDynamicsWorld(collisionDispatcher, overlappingPairCache, solver, collisionConfiguration);
	this->world->setGravity(btVector3(0.0f, PhysicsWorld::GRAVITY, 0.0f));

	this->debugDrawer = new DebugDrawer();
	this->world->setDebugDrawer(debugDrawer);
}

PhysicsWorld::~PhysicsWorld()
{
	delete debugDrawer;
	delete world;
	delete solver;
	delete overlappingPairCache;
	delete collisionDispatcher;
	delete collisionConfiguration;

	for (btRigidBody* rigidBody : this->rigidBodies)
		delete rigidBody;
}

void PhysicsWorld::update(float deltaTime)
{
	this->world->stepSimulation(deltaTime, 10);
	if (this->enableDebugDraw)
		this->world->debugDrawWorld();
}

PhysicsComponent* PhysicsWorld::raycastLine(glm::vec3 from, glm::vec3 to)
{
	btVector3 btRayFrom(from.x, from.y, from.z);
	btVector3 btRayTo(to.x, to.y, to.z);

	btCollisionWorld::ClosestRayResultCallback rayCallback(btRayFrom, btRayTo);
	this->world->rayTest(btRayFrom, btRayTo, rayCallback);

	if (rayCallback.hasHit())
	{
		btVector3 hitPoint = rayCallback.m_hitPointWorld;
		btCollisionObject* hitObject = const_cast<btCollisionObject*>(rayCallback.m_collisionObject);
		btRigidBody* hitRigidBody = btRigidBody::upcast(hitObject);

		if (this->rigidBodyToComponent.count(hitRigidBody) != 0)
			return this->rigidBodyToComponent[hitRigidBody];
	}

	return nullptr;
}

std::vector<float> PhysicsWorld::getDebugLines()
{
	std::vector<float> lines = this->debugDrawer->debugLines;
	this->debugDrawer->debugLines.clear();
	return lines;
}

void PhysicsWorld::addPlane(glm::vec3 normal, glm::vec3 position)
{
	btVector3 initialPosition(position.x, position.y, position.z);

	btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -5.0f);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, planeShape, btVector3(0.0f, 0.0f, 0.0f));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	// TODO : Properly manage deletion of plane colliders too
	this->world->addRigidBody(groundRigidBody);
}

void PhysicsWorld::addBox(PhysicsComponent* component, glm::vec3 halfExtents, glm::vec3 position, float mass, bool disableCollision)
{
	btVector3 initialPosition(position.x, position.y, position.z);
	btVector3 btHalfExtents(halfExtents.x, halfExtents.y, halfExtents.z);
	btVector3 boxInertia(0.0f, 0.0f, 0.0f);

	btCollisionShape* boxShape = new btBoxShape(btHalfExtents);
	boxShape->calculateLocalInertia(mass, boxInertia);

	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, boxInertia);
	btRigidBody* boxRigidBody = new btRigidBody(boxRigidBodyCI);

	if (disableCollision)
		boxRigidBody->setCollisionFlags(boxRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	std::unique_ptr<Collider> collider = std::make_unique<Collider>(
		this->world,
		std::unique_ptr<btCollisionShape>(boxShape),
		std::unique_ptr<btDefaultMotionState>(boxMotionState),
		std::unique_ptr<btRigidBody>(boxRigidBody)
	);

	this->world->addRigidBody(boxRigidBody);
	component->setCollider(std::move(collider));
	this->rigidBodyToComponent[boxRigidBody] = component;
}

void PhysicsWorld::addSphere(PhysicsComponent* component, float radius, glm::vec3 position, float mass, bool disableCollision)
{
	btVector3 initialPosition(position.x, position.y, position.z);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);

	btCollisionShape* sphereShape = new btSphereShape(radius);
	sphereShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphereShape, localInertia);
	btRigidBody* sphereRigidBody = new btRigidBody(rigidBodyCI);

	if (disableCollision)
		sphereRigidBody->setCollisionFlags(sphereRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	std::unique_ptr<Collider> collider = std::make_unique<Collider>(
		this->world,
		std::unique_ptr<btCollisionShape>(sphereShape),
		std::unique_ptr<btDefaultMotionState>(motionState),
		std::unique_ptr<btRigidBody>(sphereRigidBody)
	);

	this->world->addRigidBody(sphereRigidBody);
	component->setCollider(std::move(collider));
	this->rigidBodyToComponent[sphereRigidBody] = component;
}		

void PhysicsWorld::addCapsule(PhysicsComponent* component, float radius, float height, glm::vec3 position, float mass, bool disableCollision)
{
	btVector3 initialPosition(position.x, position.y, position.z);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);

	btCollisionShape* capsuleShape = new btCapsuleShape(radius, height);
	capsuleShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, capsuleShape, localInertia);
	btRigidBody* capsuleRigidBody = new btRigidBody(rigidBodyCI);

	if (disableCollision)
		capsuleRigidBody->setCollisionFlags(capsuleRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	std::unique_ptr<Collider> collider = std::make_unique<Collider>(
		this->world,
		std::unique_ptr<btCollisionShape>(capsuleShape),
		std::unique_ptr<btDefaultMotionState>(motionState),
		std::unique_ptr<btRigidBody>(capsuleRigidBody)
		);

	this->world->addRigidBody(capsuleRigidBody);
	component->setCollider(std::move(collider));
	this->rigidBodyToComponent[capsuleRigidBody] = component;
}
