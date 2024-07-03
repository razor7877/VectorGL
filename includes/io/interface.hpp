#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "skybox.hpp"
#include "renderer.hpp"

// This file is responsible for handling and displaying the various interfaces that are produced with the ImGUI library

extern float deltaTime;

void ImGuiInit(GLFWwindow* window, Renderer* rendererArg);
void ImGuiDrawWindows(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess);

// Performance info : framerate etc.
void PerformanceMenu();
// Shows the various controls
void KeysMenu();
// Enables modifications of some shader values
void ShaderSettings(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess);
// Allows changing the skybox at runtime
void SkyboxSettings();
// Shows the details of the currently selected node in the scene graph
void ShowNodeDetails();
// Displays the scene graph
void SceneGraph();
void SceneGraphRecurse(std::vector<Entity*> children);
void HandleSceneGraphClick(Entity* object);

void ShowComponentUI(Component* component);

#endif