#include "physics/physicsWorld.hpp"

PhysicsWorld::PhysicsWorld()
{
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	this->world = new btDiscreteDynamicsWorld(collisionDispatcher, overlappingPairCache, solver, collisionConfiguration);
	this->world->setGravity(btVector3(0.0f, PhysicsWorld::GRAVITY, 0.0f));

	this->debugDrawer = new DebugDrawer();
	this->world->setDebugDrawer(debugDrawer);
}

PhysicsWorld::~PhysicsWorld()
{

}

void PhysicsWorld::update(float deltaTime)
{
	this->world->stepSimulation(deltaTime, 10);
	if (this->enableDebugDraw)
		this->world->debugDrawWorld();
}

void PhysicsWorld::raycastLine(glm::vec3 from, glm::vec3 to)
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

		if (hitRigidBody && !hitRigidBody->isStaticObject() && !hitRigidBody->isKinematicObject())
		{
			btVector3 hitToCenter = hitRigidBody->getWorldTransform().getOrigin() - hitPoint;
			hitToCenter.normalize();

			constexpr float FORCE = 250.0f;

			btVector3 force = hitToCenter * FORCE;

			// Make sure rigid body is in active state then apply force at the hit point
			hitRigidBody->setActivationState(ACTIVE_TAG);
			hitRigidBody->applyForce(force, hitPoint);
		}
	}
}

std::vector<float> PhysicsWorld::getDebugLines()
{
	std::vector<float> lines = this->debugDrawer->debugLines;
	this->debugDrawer->debugLines.clear();
	return lines;
}

btRigidBody* PhysicsWorld::addPlane(glm::vec3 normal, glm::vec3 position)
{
	btVector3 initialPosition(position.x, position.y, position.z);

	btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -5.0f);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, planeShape, btVector3(0.0f, 0.0f, 0.0f));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	this->world->addRigidBody(groundRigidBody);

	return groundRigidBody;
}

btRigidBody* PhysicsWorld::addBox(glm::vec3 halfExtents, glm::vec3 position)
{
	btVector3 initialPosition(position.x, position.y, position.z);
	btVector3 btHalfExtents(halfExtents.x, halfExtents.y, halfExtents.z);
	btVector3 boxInertia(0.0f, 0.0f, 0.0f);
	btScalar mass = 1.0f;

	btCollisionShape* boxShape = new btBoxShape(btHalfExtents);
	boxShape->calculateLocalInertia(mass, boxInertia);

	btDefaultMotionState* boxMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo boxRigidBodyCI(mass, boxMotionState, boxShape, boxInertia);
	btRigidBody* boxRigidBody = new btRigidBody(boxRigidBodyCI);
	this->world->addRigidBody(boxRigidBody);

	return boxRigidBody;
}

btRigidBody* PhysicsWorld::addSphere(float radius, glm::vec3 position)
{
	btVector3 initialPosition(position.x, position.y, position.z);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	btScalar mass = 1.0f;

	btCollisionShape* sphereShape = new btSphereShape(radius);
	sphereShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), initialPosition));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphereShape, localInertia);
	btRigidBody* sphereRigidBody = new btRigidBody(rigidBodyCI);
	this->world->addRigidBody(sphereRigidBody);

	return sphereRigidBody;
}
