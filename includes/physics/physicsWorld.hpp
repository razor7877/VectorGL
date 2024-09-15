#pragma once

#include <vector>
#include <map>
#include <memory>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>

#include <components/physicsComponent.hpp>

class DebugDrawer : public btIDebugDraw
{
public:
	std::vector<float> debugLines;

	DebugDrawer() : m_debugMode(DBG_DrawWireframe) {}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		this->debugLines.push_back(from.x());
		this->debugLines.push_back(from.y());
		this->debugLines.push_back(from.z());
		this->debugLines.push_back(to.x());
		this->debugLines.push_back(to.y());
		this->debugLines.push_back(to.z());
	}

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		// Optionally implement this to draw contact points
	}

	virtual void reportErrorWarning(const char* warningString)
	{
		printf("%s\n", warningString);
	}

	virtual void draw3dText(const btVector3& location, const char* textString)
	{
		// Optionally implement this to draw 3D text
	}

	virtual void setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	virtual int getDebugMode() const
	{
		return m_debugMode;
	}

private:
	int m_debugMode;
};

class PhysicsWorld
{
public:
	std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> collisionDispatcher;
	std::unique_ptr<btBroadphaseInterface> overlappingPairCache;
	std::unique_ptr<btGhostPairCallback> ghostPairCallback;
	std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
	std::unique_ptr<btDiscreteDynamicsWorld> world;

	static constexpr float GRAVITY = -9.81f;
	bool enableDebugDraw = false;

	PhysicsWorld();
	~PhysicsWorld();

	/// <summary>
	/// Updates the physics simulation
	/// </summary>
	/// <param name="deltaTime">The time elapsed since the last frame</param>
	void update(float deltaTime);

	/// <summary>
	/// Projects a ray and checks for collisions
	/// </summary>
	/// <param name="from">The ray start position</param>
	/// <param name="to">The ray end position</param>
	/// <returns>A pointer to the first object that was hit, or nullptr if not hit found</returns>
	PhysicsComponent* raycastLine(glm::vec3 from, glm::vec3 to);

	std::vector<float> getDebugLines();

	/// <summary>
	/// Creates a new plane collider
	/// </summary>
	/// <param name="normal">The normal of the plane</param>
	/// <param name="position">The position of the plane</param>
	void addPlane(glm::vec3 normal, glm::vec3 position);

	/// <summary>
	/// Creates a new box collider
	/// </summary>
	/// <param name="component">The physics component of the entity for which the collider is added</param>
	/// <param name="halfExtents">The half extents of the box</param>
	/// <param name="position">The position of the box</param>
	/// <param name="mass">The mass of the box</param>
	/// <param name="disableCollision">Whether collision testing should be disabled (if colliders are for raycasts only for example)</param>
	void addBox(PhysicsComponent* component, glm::vec3 halfExtents, glm::vec3 position, float mass = 1.0f, bool disableCollision = false);

	/// <summary>
	/// Creates a new sphere collider
	/// </summary>
	/// <param name="component">The physics component of the entity for which the collider is added</param>
	/// <param name="radius">The radius of the sphere</param>
	/// <param name="position">The position of the sphere</param>
	/// <param name="mass">The mass of the sphere</param>
	/// <param name="disableCollision">Whether collision testing should be disabled (if colliders are for raycasts only for example)</param>
	void addSphere(PhysicsComponent* component, float radius, glm::vec3 position, float mass = 1.0f, bool disableCollision = false);

	/// <summary>
	/// Creates a new capsule collider
	/// </summary>
	/// <param name="component">The physics component of the entity for which the collider is added</param>
	/// <param name="radius">The radius of the capsule</param>
	/// <param name="height">The height of the capsule</param>
	/// <param name="position">The position of the capsule</param>
	/// <param name="mass">The mass of the capsule</param>
	/// <param name="disableCollision">Whether collision testing should be disabled (if colliders are for raycasts only for example)</param>
	void addCapsule(PhysicsComponent* component, float radius, float height, glm::vec3 position, float mass = 1.0f, bool disableCollision = false);

private:
	std::unique_ptr<DebugDrawer> debugDrawer;

	std::vector<btRigidBody*> rigidBodies;
	std::map<btRigidBody*, PhysicsComponent*> rigidBodyToComponent;
};