#ifndef MAIN_HPP
#define MAIN_HPP

// This is the main file, responsible for importing other header files, setting up context, necessary objects, and executing the render loop

#include <utilities/glad.h>
#include <GLFW/glfw3.h>

#include "components/cameraComponent.hpp"
#include "renderer.hpp"
#include "texture.hpp"
#include "cubemap.hpp"

extern int windowWidth;
extern int windowHeight;

extern Renderer defaultRenderer;
extern CameraComponent* cameraComponent;

void raycastLine(glm::vec3 rayFrom, glm::vec3 rayTo);

int setupGlfwContext();
int main();

#endif