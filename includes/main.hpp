#ifndef MAIN_HPP
#define MAIN_HPP

// This is the main file, responsible for importing other header files, setting up context, necessary objects, and executing the render loop

#include "game/gameEngine.hpp"

namespace Main
{
	extern GameEngine& game;
	extern float deltaTime;
	extern float lastFrame;
}

#endif