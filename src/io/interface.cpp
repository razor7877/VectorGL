#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
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

// The currently selected node in the scene graph
RenderObject* selectedSceneNode{};

std::string editLabel{};

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
	ShowNodeDetails();
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
		ImGui::PushID(dirLights[i]);

		if (ImGui::CollapsingHeader("DirectionalLight"))
		{
			bool modified = false;

			if (ImGui::ColorEdit3("DL_Amb", &(dirLights[i]->ambientColor[0])))
				modified = true;
			if (ImGui::ColorEdit3("DL_Diff", &(dirLights[i]->diffuseColor[0])))
				modified = true;
			if (ImGui::ColorEdit3("DL_Spec", &(dirLights[i]->specularColor[0])))
				modified = true;

			if (ImGui::DragFloat3("DL_Dir", &(dirLights[i]->direction[0]), 0.002f))
				modified = true;

			if (modified)
			{
				glUseProgram(lightManager.shaderProgramID);
				dirLights[i]->sendToShader(lightManager.shaderProgramID, i);
			}
		}

		ImGui::PopID();
	}

	for (int i = 0; i < pointLights.size(); i++)
	{
		ImGui::PushID(pointLights[i]);

		if (ImGui::CollapsingHeader("PointLight"))
		{
			bool modified = false;

			if (ImGui::ColorEdit3("PL_Amb", &(pointLights[i]->ambientColor[0])))
				modified = true;
			if (ImGui::ColorEdit3("PL_Diff", &(pointLights[i]->diffuseColor[0])))
				modified = true;
			if (ImGui::ColorEdit3("PL_Spec", &(pointLights[i]->specularColor[0])))
				modified = true;

			if (ImGui::DragFloat3("PL_Pos", &(pointLights[i]->position[0])))
				modified = true;

			if (ImGui::DragFloat("PL_Const", &(pointLights[i]->constant), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat("PL_Lin", &(pointLights[i]->linear), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat("PL_Quad", &(pointLights[i]->quadratic), 0.002f, 0.001f, 1.0f))
				modified = true;

			if (modified)
			{
				glUseProgram(lightManager.shaderProgramID);
				pointLights[i]->sendToShader(lightManager.shaderProgramID, i);
			}
		}

		ImGui::PopID();
	}

	for (int i = 0; i < spotLights.size(); i++)
	{
		ImGui::PushID(spotLights[i]);

		if (ImGui::CollapsingHeader("SpotLight"))
		{
			bool modified = false;

			if (ImGui::ColorEdit3("SL_Amb", &(spotLights[i]->ambientColor[0])))
				modified = true;
			if (ImGui::ColorEdit3("SL_Diff", &(spotLights[i]->diffuseColor[0])))
				modified = true;
			if (ImGui::ColorEdit3("SL_Spec", &(spotLights[i]->specularColor[0])))
				modified = true;

			if (ImGui::DragFloat3("SL_Pos", &(spotLights[i]->position[0])))
				modified = true;
			if (ImGui::DragFloat3("SL_Dir", &(spotLights[i]->direction[0]), 0.002f))
				modified = true;

			if (ImGui::DragFloat("SL_Const", &(spotLights[i]->constant), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat("SL_Lin", &(spotLights[i]->linear), 0.002f, 0.0f, 1.0f))
				modified = true;
			if (ImGui::DragFloat("SL_Quad", &(spotLights[i]->quadratic), 0.002f, 0.001f, 1.0f))
				modified = true;

			if (ImGui::DragFloat("SL_cutOff", &(spotLights[i]->cutOff), 0.002f, 0.001f, 1.0f))
				modified = true;
			if (ImGui::DragFloat("SL_outerCutOff", &(spotLights[i]->outerCutOff), 0.002f, 0.001f, 1.0f))
				modified = true;

			if (modified)
			{
				glUseProgram(lightManager.shaderProgramID);
				spotLights[i]->sendToShader(lightManager.shaderProgramID, i);
			}
		}

		ImGui::PopID();
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

				delete skybox->cubemap;

				switch (item_current_idx)
				{
					case 0:
						skybox->cubemap = new Cubemap("img/skybox/grass/");
						break;

					case 1:
						skybox->cubemap = new Cubemap("img/skybox/night/");
						break;

					case 2:
						skybox->cubemap = new Cubemap("img/skybox/sky/");
						break;
				}

				skybox->cubemap->setupObject();
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::End();
}

// TODO : Add rotation
void ShowNodeDetails()
{
	ImGui::Begin("Node details");

	if (selectedSceneNode != nullptr)
	{
		ImGui::Text(selectedSceneNode->getLabel().c_str());

		glm::vec3 position = selectedSceneNode->getPosition();
		if (ImGui::DragFloat3("Position", &position[0]), 0.10f)
			selectedSceneNode->setPosition(position);

		glm::vec3 rotation = selectedSceneNode->getRotation();
		if (ImGui::DragFloat3("Rotation", &rotation[0]))
			selectedSceneNode->setRotation(rotation);

		glm::vec3 scale = selectedSceneNode->getScale();
		if (ImGui::DragFloat3("Scale", &scale[0], 0.01f))
			selectedSceneNode->setScale(scale);

		bool isVisible = selectedSceneNode->getIsVisible();
		if (ImGui::Checkbox("Visible", &isVisible))
			selectedSceneNode->setIsVisible(isVisible);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("Delete object"))
		{
			renderer->removeObject(selectedSceneNode);

			delete selectedSceneNode;
			selectedSceneNode = nullptr;
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	ImGui::End();
}

void SceneGraph()
{
	ImGui::Begin("Scene graph");

	if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
	{
		SceneGraphRecurse(renderer->objects);
		ImGui::TreePop();
	}

	ImGui::End();
}

void SceneGraphRecurse(std::vector<RenderObject*> children)
{
	for (RenderObject* child : children)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;

		// Different flags for tree and leaf nodes
		if (child->getChildren().size() == 0)
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		else
		{
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		}

		// Highlight selected node
		if (selectedSceneNode == child)
			flags |= ImGuiTreeNodeFlags_Selected;
		
		// Change text color for hidden nodes
		if (!child->getIsVisible())
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));

		ImGui::PushID(child); // Pushing ID to avoid conflict of nodes with same name

		if (ImGui::TreeNodeEx(child->getLabel().c_str(), flags))
		{
			// Detects clicks on opened tree node
			HandleSceneGraphClick(child);

			SceneGraphRecurse(child->getChildren());
			ImGui::TreePop();
		}
		else
		{
			// Detects clicks on unopened tree node
			HandleSceneGraphClick(child);
		}

		ImGui::PopID();
		
		if (!child->getIsVisible())
			ImGui::PopStyleColor();
	}
}

void HandleSceneGraphClick(RenderObject* object)
{
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		selectedSceneNode = object;
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("NodePopup");

	if (ImGui::BeginPopup("NodePopup"))
	{
		if (ImGui::Button("Rename"))
			ImGui::OpenPopup("RenamePopup");

		if (ImGui::BeginPopup("RenamePopup"))
		{
			ImGui::SetKeyboardFocusHere(0);
			editLabel = object->getLabel();
			
			if (ImGui::InputText("##editLabel", &editLabel, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				object->setLabel(editLabel);
			}

			ImGui::EndPopup();
		}

		if (ImGui::Button("Delete"))
		{
			if (object == selectedSceneNode)
				selectedSceneNode = nullptr;

			renderer->removeObject(object);
			delete object;
		}
			

		ImGui::EndPopup();
	}
}