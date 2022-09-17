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

const char* items[] = { "Grass", "Night", "Sky" };
static int item_current_idx = 2; // Selection data index

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

void ImGuiDrawWindows(Camera& camera, glm::vec3& position, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess, Cubemap& cubemap)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	CameraMenu(camera);
	PerformanceMenu();
	KeysMenu();
	ShaderSettings(ambient, diffuse, specular, shininess);
	LightSettings(position);
	SkyboxSettings(cubemap);

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

void ShaderSettings(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess)
{
	ImGui::Begin("Shader settings");

	ImGui::Text("Ambient");
	ImGui::PushItemWidth(50.0f);
	ImGui::SliderFloat("R", &ambient[0], 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::SliderFloat("G", &ambient[1], 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::SliderFloat("B", &ambient[2], 0.0f, 1.0f);
	ImGui::NewLine();

	ImGui::Text("Diffuse");
	ImGui::PushItemWidth(50.0f);
	ImGui::SliderFloat("R", &diffuse[0], 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::SliderFloat("G", &diffuse[1], 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::SliderFloat("B", &diffuse[2], 0.0f, 1.0f);
	ImGui::NewLine();

	ImGui::Text("Specular");
	ImGui::PushItemWidth(50.0f);
	ImGui::SliderFloat("R", &specular[0], 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::SliderFloat("G", &specular[1], 0.0f, 1.0f);
	ImGui::SameLine();
	ImGui::SliderFloat("B", &specular[2], 0.0f, 1.0f);
	ImGui::NewLine();

	ImGui::SliderFloat("Shininess", &shininess, 0.0f, 512.0f);

	ImGui::End();
}

void LightSettings(glm::vec3& position)
{
	ImGui::Begin("Light settings");

	ImGui::Text("Position :");
	ImGui::PushItemWidth(50.0f);
	ImGui::InputFloat("X", &position[0]);
	ImGui::SameLine();
	ImGui::InputFloat("Y", &position[1]);
	ImGui::SameLine();
	ImGui::InputFloat("Z", &position[2]);

	ImGui::End();
}

void SkyboxSettings(Cubemap& cubemap)
{
	ImGui::Begin("Skybox settings");

	const char* combo_preview_value = items[item_current_idx]; // Pass in the preview value visible before opening the combo
	if (ImGui::BeginCombo("Skybox", combo_preview_value))
	{
		for (int i = 0; i < IM_ARRAYSIZE(items); i++)
		{
			const bool is_selected = (item_current_idx == i);
			if (ImGui::Selectable(items[i], is_selected))
			{
				item_current_idx = i;

				switch (item_current_idx)
				{
					case 0:
						cubemap = Cubemap("img/skybox/grass/");
						break;

					case 1:
						cubemap = Cubemap("img/skybox/night/");
						break;

					case 2:
						cubemap = Cubemap("img/skybox/sky/");
						break;
				}
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::End();
}