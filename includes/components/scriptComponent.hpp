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
	std::string scriptCode = "";

	ScriptComponent(Entity* parent);
	~ScriptComponent() override;

	void Component::start() override;
	void Component::update(float deltaTime) override;

private:
	lua_State* L;
};