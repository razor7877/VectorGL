#include "components/scriptComponent.hpp"
#include "logger.hpp"

ScriptComponent::ScriptComponent(Entity* parent) : Component(parent)
{
	this->L = luaL_newstate();
	luaL_openlibs(this->L);
}

ScriptComponent::~ScriptComponent()
{
	lua_close(this->L);
}

void ScriptComponent::start()
{
	
}

void ScriptComponent::update()
{
	// Load and run lua script
	int error = luaL_loadbuffer(this->L, this->scriptCode.c_str(), this->scriptCode.size(), "line") || lua_pcall(this->L, 0, 0, 0);
	if (error)
	{
		std::string errorMessage = lua_tostring(this->L, -1);
		Logger::logError("Lua error: " + errorMessage);
		// Pop error from stack
		lua_pop(this->L, 1);
	}
}
