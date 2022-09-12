#include <string>
#include <cstring>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "headers/main.hpp"
#include "headers/interface.hpp"
#include "headers/camera.hpp"

float lastFrames[100];
int frameIndex{};
float timeToFrame = 1;

void ImGuiInit(GLFWwindow* window)
{
	// Setup Dear ImgGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup platform/renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Setup Dear ImgUi style
	ImGui::StyleColorsDark();
}

void ImGuiDrawWindows(Camera &camera)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	CameraMenu(camera);
	PerformanceMenu();
	KeysMenu();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CameraMenu(Camera& camera)
{
	ImGui::Begin("Camera");

	// Camera FOV value
	ImGui::PushItemWidth(100.0f);
	ImGui::SliderFloat("FOV", &camera.zoom, 1.0f, 90.0f);
	ImGui::NewLine();

	// Camera xyz position
	ImGui::Text("Position");
	ImGui::PushItemWidth(50.0f);
	ImGui::InputFloat("X", &camera.position[0]);
	ImGui::SameLine();
	ImGui::InputFloat("Y", &camera.position[1]);
	ImGui::SameLine();
	ImGui::InputFloat("Z", &camera.position[2]);
	ImGui::NewLine();

	ImGui::PushItemWidth(100.0f);
	ImGui::InputFloat("Speed", &camera.movementSpeed);
	ImGui::InputFloat("Sensitivity", &camera.mouseSensitivity);

	ImGui::NewLine();
	if (ImGui::Button("Reset"))
	{
		camera.zoom = ZOOM;
		camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
		camera.movementSpeed = SPEED;
		camera.mouseSensitivity = SENSITIVITY;
	}

	ImGui::End();
}

void PerformanceMenu()
{
	ImGui::Begin("Performance");

	ImGui::Text("Frame render time: %.2f ms", timeToFrame * 1000);;

	lastFrames[frameIndex] = deltaTime;
	frameIndex++;
	
	if (frameIndex == 100)
	{
		frameIndex = 0;
		float frameTimeSum{};
		for (int i = 0; i < 100; i++)
		{
			frameTimeSum += lastFrames[i];
		}
		timeToFrame = frameTimeSum / 100;
	}

	ImGui::Text("Framerate: %i", (int)(1 / timeToFrame));

	ImGui::End();
}

void KeysMenu()
{
	ImGui::Begin("Controls");

	ImGui::Text("Look around : Mouse");
	ImGui::Text("Movement : WASD");
	ImGui::Text("Toggle cursor : Left control");
	ImGui::Text("Change FOV : Scroll wheel");
	ImGui::Text("Toggle fullscreen : F11");
	ImGui::Text("Quit : Esc");

	ImGui::End();
}