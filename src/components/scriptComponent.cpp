#include "components/scriptComponent.hpp"
#include "logger.hpp"
#include "entity.hpp"

ScriptComponent::ScriptComponent(Entity* parent) : Component(parent)
{
	this->L = luaL_newstate();
	luaL_openlibs(this->L);

	lua_pushnumber(L, this->parent->transform->getPosition().x);
	lua_setglobal(L, "xPos");
}

ScriptComponent::~ScriptComponent()
{
	lua_close(this->L);
}

void ScriptComponent::start()
{
	
}

void ScriptComponent::update(float deltaTime)
{
	// Push delta time as Lua global variable
	lua_pushnumber(L, deltaTime);
	lua_setglobal(L, "deltaTime");

	// Load and run lua script
	int error = luaL_loadbuffer(this->L, this->scriptCode.c_str(), this->scriptCode.size(), "line") || lua_pcall(this->L, 0, 0, 0);
	if (error)
	{
		std::string errorMessage = lua_tostring(this->L, -1);
		Logger::logError("Lua error: " + errorMessage, "scriptComponent.cpp");
		// Pop error from stack
		lua_pop(this->L, 1);
	}

	// Retrieve updated xPos value
	lua_getglobal(L, "xPos");
	if (lua_isnumber(L, -1))
	{
		float newX = lua_tonumber(L, -1);
		glm::vec3 transformPos = this->parent->transform->getPosition();
		this->parent->transform->setPosition(glm::vec3(newX, transformPos.y, transformPos.z));
	}
}
