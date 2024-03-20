#ifndef MAIN_HPP
#define MAIN_HPP

// This is the main file, responsible for importing other header files, setting up context, necessary objects, and executing the render loop

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "renderer.hpp"

extern int windowWidth;
extern int windowHeight;

extern Camera camera;

void initUniformBuffer();
void updateUniformBuffer(glm::mat4 view, glm::mat4 projection);
int setupGlfwContext();
int main();

#endif