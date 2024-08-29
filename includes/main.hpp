#ifndef MAIN_HPP
#define MAIN_HPP

// This is the main file, responsible for importing other header files, setting up context, necessary objects, and executing the render loop

#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "components/cameraComponent.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "cubemap.hpp"
#include "game/gameEngine.hpp"

extern int windowWidth;
extern int windowHeight;

extern GameEngine& game;

int setupGlfwContext();
int main();

#endif