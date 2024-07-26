#pragma once

#include <vector>

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

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
	static constexpr float GRAVITY = -9.81f;

	bool enableDebugDraw = false;

	PhysicsWorld();
	~PhysicsWorld();

	void update(float deltaTime);
	void raycastLine(glm::vec3 from, glm::vec3 to);

	std::vector<float> getDebugLines();

	btRigidBody* addPlane(glm::vec3 normal, glm::vec3 position);
	btRigidBody* addBox(glm::vec3 halfExtents, glm::vec3 position);
	btRigidBody* addSphere(float radius, glm::vec3 position);

private:
	btDiscreteDynamicsWorld* world;
	DebugDrawer* debugDrawer;

	std::vector<btRigidBody*> rigidBodies;
};