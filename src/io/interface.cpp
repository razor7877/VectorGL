#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "main.hpp"
#include "io/interface.hpp"
#include "lights/lightManager.hpp"
#include "lights/directionalLight.hpp"
#include "lights/pointLight.hpp"
#include "lights/spotLight.hpp"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

// NVIDIA GPU only, serves to query the amount of total and available VRAM
int total_mem_kb = 0;
int cur_avail_mem_kb = 0;
const char* gpuVendor{};
std::string gpuVendorStr{};
bool isNvidiaGpu = false;

// For calculating the FPS, the time taken to render the last 100 frames is used
float lastFrames[100];
int frameIndex{};
float timeToFrame = 1;

// Used for dynamically showing existing lights and enabling their realtime modification
Renderer* renderer;
LightManager lightManager;
std::vector<DirectionalLight*> dirLights;
std::vector<PointLight*> pointLights;
std::vector<SpotLight*> spotLights;

// An array containing the choice names for the different skyboxes
const char* items[] = { "Grass", "Night", "Sky" };
static int item_current_idx = 1; // Selection data index

void ImGuiInit(GLFWwindow* window, Renderer* rendererArg)
{
	// OpenGL context needs to be initalized beforehand to call glGetString()
	gpuVendor = (char*)glGetString(GL_VENDOR);
	gpuVendorStr = std::string(gpuVendor);
	isNvidiaGpu = gpuVendorStr == "Nvidia";

	// Setup Dear ImgGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup platform/renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	// Setup Dear ImgUi style
	ImGui::StyleColorsDark();

	renderer = rendererArg;
	lightManager = renderer->lightManager;
	dirLights = lightManager.getDirLights();
	pointLights = lightManager.getPointLights();
	spotLights = lightManager.getSpotLights();
}

void ImGuiDrawWindows(Camera& camera, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess, Skybox* skybox)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	CameraMenu(camera);
	PerformanceMenu();
	KeysMenu();
	ShaderSettings(ambient, diffuse, specular, shininess);
	LightSettings();
	SkyboxSettings(skybox);
	SceneGraph();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void CameraMenu(Camera& camera)
{
	ImGui::Begin("Camera");

	// Camera FOV value
	ImGui::PushItemWidth(100.0f);
	ImGui::SliderFloat("FOV", &camera.zoom, 1.0f, 90.0f);

	ImGui::PushItemWidth(150.0f);
	ImGui::DragFloat3("Position", &camera.position[0]);

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

	if (isNvidiaGpu)
	{
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb);
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb);

		ImGui::Text("Total VRAM (MB): %i", total_mem_kb / 1000);
		ImGui::Text("Available VRAM (MB): %i", cur_avail_mem_kb / 1000);
		ImGui::Text("Used VRAM (MB): %i", (total_mem_kb - cur_avail_mem_kb) / 1000);
	}

	ImGui::End();
}

void KeysMenu()
{
	ImGui::Begin("Controls");

	ImGui::Text("Look around : Mouse");
	ImGui::Text("Movement : WASD");
	ImGui::Text("Toggle cursor : Left control");
	ImGui::Text("Change FOV : Scroll wheel");
	ImGui::Text("Toggle wireframe : F10");
	ImGui::Text("Toggle fullscreen : F11");
	ImGui::Text("Quit : Esc");

	ImGui::End();
}

void ShaderSettings(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float& shininess)
{
	ImGui::Begin("Shader settings");

	ImGui::ColorEdit3("Ambient", &ambient[0]);
	ImGui::ColorEdit3("Diffuse", &diffuse[0]);
	ImGui::ColorEdit3("Specular", &specular[0]);

	ImGui::DragFloat("Shininess", &shininess, 1.0f, 1.0f, 512.0f);

	ImGui::End();
}

void LightSettings()
{
	ImGui::Begin("Light settings");

	for (int i = 0; i < dirLights.size(); i++)
	{
		if (ImGui::CollapsingHeader(("DirectionalLight " + std::to_string(i)).c_str()))
		{
			bool modified = false;

			if (ImGui::ColorEdit3(("DL_Amb " + std::to_string(i)).c_str(), &(dirLights[i]->ambientColor[0])))
				modified = true;
			if (ImGui::ColorEdit3(("DL_Diff " + std::to_string(i)).c_str(), &(dirLights[i]->diffuseColor[0])))
				modified = true;
			if (ImGui::ColorEdit3(("DL_Spec " + std::to_string(i)).c_str(), &(dirLights[i]->specularColor[0])))
				modified = true;

			if (ImGui::DragFloat3(("DL_Dir " + std::to_string(i)).c_str(), &(dirLights[i]->direction[0]), 0.002f))
				modified = true;

			if (modified)
			{
				glUseProgram(lightManager.shaderProgramID);
				dirLights[i]->sendToShader(lightManager.shaderProgramID, i);
			}
		}
	}

	for (int i = 0; i < pointLights.size(); i++)
	{
		if (ImGui::CollapsingHeader(("PointLight " + std::to_string(i)).c_str()))
		{
			bool modified = false;

			if (ImGui::ColorEdit3(("PL_Amb " + std::to_string(i)).c_str(), &(pointLights[i]->ambientColor[0])))
				modified = true;
			if (ImGui::ColorEdit3(("PL_Diff " + std::to_string(i)).c_str(), &(pointLights[i]->diffuseColor[0])))
				modified = true;
			if (ImGui::ColorEdit3(("PL_Spec " + std::to_string(i)).c_str(), &(pointLights[i]->specularColor[0])))
				modified = true;

			if (ImGui::DragFloat3(("PL_Pos " + std::to_string(i)).c_str(), &(pointLights[i]->position[0])))
				modified = true;

			if (ImGui::DragFloat(("PL_Const " + std::to_string(i)).c_str(), &(pointLights[i]->constant), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat(("PL_Lin " + std::to_string(i)).c_str(), &(pointLights[i]->linear), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat(("PL_Quad " + std::to_string(i)).c_str(), &(pointLights[i]->quadratic), 0.002f, 0.001f, 1.0f))
				modified = true;

			if (modified)
			{
				glUseProgram(lightManager.shaderProgramID);
				pointLights[i]->sendToShader(lightManager.shaderProgramID, i);
			}
		}
	}

	for (int i = 0; i < spotLights.size(); i++)
	{
		if (ImGui::CollapsingHeader(("SpotLight " + std::to_string(i)).c_str()))
		{
			bool modified = false;

			if (ImGui::ColorEdit3(("SL_Amb " + std::to_string(i)).c_str(), &(spotLights[i]->ambientColor[0])))
				modified = true;
			if (ImGui::ColorEdit3(("SL_Diff " + std::to_string(i)).c_str(), &(spotLights[i]->diffuseColor[0])))
				modified = true;
			if (ImGui::ColorEdit3(("SL_Spec " + std::to_string(i)).c_str(), &(spotLights[i]->specularColor[0])))
				modified = true;

			if (ImGui::DragFloat3(("SL_Pos " + std::to_string(i)).c_str(), &(spotLights[i]->position[0])))
				modified = true;
			if (ImGui::DragFloat3(("SL_Dir " + std::to_string(i)).c_str(), &(spotLights[i]->direction[0]), 0.002f))
				modified = true;

			if (ImGui::DragFloat(("SL_Const " + std::to_string(i)).c_str(), &(spotLights[i]->constant), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat(("SL_Lin " + std::to_string(i)).c_str(), &(spotLights[i]->linear), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat(("SL_Quad " + std::to_string(i)).c_str(), &(spotLights[i]->quadratic), 0.002f, 0.001f, 1.0f))
				modified = true;

			if (ImGui::DragFloat(("SL_cutOff " + std::to_string(i)).c_str(), &(spotLights[i]->cutOff), 0.002f, 0.001f, 1.0f))
				modified = true;
			if (ImGui::DragFloat(("SL_outerCutOff " + std::to_string(i)).c_str(), &(spotLights[i]->outerCutOff), 0.002f, 0.001f, 1.0f))
				modified = true;

			if (modified)
			{
				glUseProgram(lightManager.shaderProgramID);
				spotLights[i]->sendToShader(lightManager.shaderProgramID, i);
			}
		}
	}

	ImGui::End();
}

void SkyboxSettings(Skybox* skybox)
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

				std::cout << skybox->cubemap->texID << std::endl;

				switch (item_current_idx)
				{
					case 0:
						//skybox->cubemap = Cubemap("img/skybox/grass/");
						break;

					case 1:
						//skybox->cubemap = Cubemap("img/skybox/night/");
						break;

					case 2:
						//skybox->cubemap = Cubemap("img/skybox/sky/");
						break;
				}

				skybox->setupObject();

			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::End();
}

void SceneGraph()
{
	for (RenderObject* node : renderer->objects)
	{
		if (ImGui::CollapsingHeader(node->getLabel().c_str())) {}
	}
}