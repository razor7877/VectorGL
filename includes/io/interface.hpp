#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "renderer.hpp"

// This file is responsible for handling and displaying the various interfaces that are produced with the ImGUI library

extern bool isViewerFocused;

/// <summary>
/// Initializes ImGui
/// </summary>
/// <param name="window">A pointer to the application window</param>
/// <param name="rendererArg">The renderer used to render the scene</param>
void ImGuiInit(GLFWwindow* window, Renderer* rendererArg);

/// <summary>
/// Draws all the ImGui windows
/// </summary>
void ImGuiDrawWindows();

/// <summary>
/// The window with the 3D viewer
/// </summary>
void ShowViewer();

/// <summary>
/// Log console
/// </summary>
void ShowConsole();

/// <summary>
/// Performance info : framerate etc.
/// </summary>
void PerformanceMenu();

/// <summary>
/// Shows the various controls
/// </summary>
void KeysMenu();

/// <summary>
/// The window that allows selecting a shader to edit as well as changing some shader parameters directly
/// </summary>
void ShaderSettings();

/// <summary>
/// Called to setup the code editor
/// </summary>
void SetupEditor();

/// <summary>
/// The window that displays the code editor to edit shaders
/// </summary>
void ShowEditor();

/// <summary>
/// Shows the details of the currently selected node in the scene graph
/// </summary>
void ShowNodeDetails();

/// <summary>
/// Displays the scene graph
/// </summary>
void SceneGraph();

/// <summary>
/// Called to recursively get and display the children of a node
/// </summary>
/// <param name="children">The list of children of a node</param>
void SceneGraphRecurse(std::vector<Entity*> children);

/// <summary>
/// Handles selection and right-clicking menus on scene nodes in scene graph
/// </summary>
/// <param name="object">The object that was interacted with</param>
void HandleSceneGraphClick(Entity* object);

/// <summary>
/// Shows the UI for any given component
/// </summary>
/// <param name="component">The component for which the UI should be displayed</param>
void ShowComponentUI(Component* component);

void TextureViewer();

#endif