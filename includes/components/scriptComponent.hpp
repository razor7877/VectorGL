#pragma once

#include <string>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "components/component.hpp"

class ScriptComponent : public virtual Component
{
public:
	std::string scriptCode;

	explicit ScriptComponent(Entity* parent);
	~ScriptComponent() override;

	void start() override;
	void update(float deltaTime) override;

private:
	lua_State* L;
};
