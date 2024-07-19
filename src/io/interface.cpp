#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "ImGuiColorTextEdit/TextEditor.h"

#include "main.hpp"
#include "entity.hpp"
#include "io/interface.hpp"
#include "lights/lightManager.hpp"
#include "logger.hpp"

#include "components/meshComponent.hpp"
#include "components/transformComponent.hpp"
#include "components/cameraComponent.hpp"
#include "components/lights/lightComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/lights/spotLightComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/skyboxComponent.hpp"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

// Used for dynamically showing existing lights and enabling their realtime modification
Renderer* renderer;

// The currently selected node in the scene graph
Entity* selectedSceneNode{};

std::string editLabel{};

// For the shader editor
TextEditor editor;
static const char* fileToEdit;

bool isViewerFocused = false;

extern float deltaTime;

struct
{
	// An array containing the choice names for the different skyboxes
	const char* comboSkyboxes[3] = { "Grass", "Night", "Sky" };
	int current_skybox_id = (int)SkyboxComponent::DEFAULT_SKY;
} skyboxParams;

struct
{
	// For calculating the FPS, the time taken to render the last 100 frames is used
	float lastFrames[100];
	int frameIndex{};
	float timeToFrame = 1;

	// NVIDIA GPU only, serves to query the amount of total and available VRAM
	int total_mem_kb = 0;
	int cur_avail_mem_kb = 0;
	const char* gpuVendor{};
	std::string gpuVendorStr{};
	bool isNvidiaGpu = false;
} performanceParams;

struct
{
	// A map to determine text color depending on log type
	std::map<LogLevel, ImVec4> logToColor =
	{
		{ LogLevel::LOG_INFO, ImVec4(0.8f, 0.8f, 0.8f, 1.0f) },
		{ LogLevel::LOG_WARNING, ImVec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ LogLevel::LOG_ERROR, ImVec4(1.0f, 0.1f, 0.1f, 1.0f) },
		{ LogLevel::LOG_DEBUG, ImVec4(0.1f, 0.1f, 1.0f, 1.0f) },
	};
	// An array for the log filter choices
	const char* comboLogLevels[5] = { "All", "Info", "Warning", "Error", "Debug" };
	// The currently selected log filter
	int current_log_filter_id = 0;
} consoleParams;

struct
{
	std::string currentEditedShaderPath;
	ShaderType currentEditedShaderType;
	bool isEditingShader = false;
	bool isEditingVertexShader = false;

	float roughness = 0.0f;
	float metallic = 0.0f;
	float ao = 0.0f;
} shaderSettingsParams;


struct
{
	Texture* currentTexture = nullptr;
	bool showTexture = false;
} textureViewerParams;


void ImGuiInit(GLFWwindow* window, Renderer* rendererArg)
{
	// OpenGL context needs to be initalized beforehand to call glGetString()
	performanceParams.gpuVendor = (char*)glGetString(GL_VENDOR);
	performanceParams.gpuVendorStr = std::string(performanceParams.gpuVendor);
	performanceParams.isNvidiaGpu = performanceParams.gpuVendorStr == "Nvidia";

	// Setup Dear ImgGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup platform/renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Setup Dear ImgUi style
	ImGui::StyleColorsDark();

	SetupEditor();

	renderer = rendererArg;
}

bool showTest = false;

void ImGuiDrawWindows()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::BeginMainMenuBar();
	ImVec2 mainMenuBarSize = ImGui::GetWindowSize();
	ImGui::MenuItem("Test", NULL, &showTest);
	ImGui::EndMainMenuBar();

	// We want to create a full size window
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarSize.y));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight - mainMenuBarSize.y));
	ImGui::Begin("Main Window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

	// We can now show all the different dockable windows
	ShowViewer();
	ShowConsole();
	PerformanceMenu();
	KeysMenu();
	ShaderSettings();
	ShowEditor();
	ShowNodeDetails();
	SceneGraph();
	TextureViewer();

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ShowViewer()
{
	ImGui::Begin("Viewer");

	isViewerFocused = ImGui::IsWindowFocused();

	if (isViewerFocused)
		cameraComponent->parent->setIsEnabled(true);
	else
		cameraComponent->parent->setIsEnabled(false);

	ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();

	defaultRenderer.resizeFramebuffer(glm::vec2(contentRegionSize.x, contentRegionSize.y));

	ImGui::Image(
		(ImTextureID)renderer->GetRenderTexture(),
		contentRegionSize,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	ImGui::End();
}

void ShowConsole()
{
	ImGui::Begin("Console");

	ImGui::BeginChild("Logs", ImVec2(0, 150), 0, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.2f, 0.2f, 0.5f));

	std::vector<Log> logs;

	if (consoleParams.current_log_filter_id == 0)
		logs = Logger::getLogs();
	else if (consoleParams.current_log_filter_id == 1)
		logs = Logger::getLogs(LogLevel::LOG_INFO);
	else if (consoleParams.current_log_filter_id == 2)
		logs = Logger::getLogs(LogLevel::LOG_WARNING);
	else if (consoleParams.current_log_filter_id == 3)
		logs = Logger::getLogs(LogLevel::LOG_ERROR);
	else if (consoleParams.current_log_filter_id == 4)
		logs = Logger::getLogs(LogLevel::LOG_DEBUG);

	for (int i = logs.size() - 1; i > 0; i--)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, consoleParams.logToColor[logs[i].logLevel]);
		ImGui::Text(logs[i].logMessage.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleColor();
	ImGui::EndChild();

	if (ImGui::Button("Clear logs"))
		Logger::clearLogs();

	ImGui::SameLine();
	ImGui::PushItemWidth(200);

	if (ImGui::BeginCombo("##logFilterCombo", consoleParams.comboLogLevels[consoleParams.current_log_filter_id]))
	{
		for (int i = 0; i < IM_ARRAYSIZE(consoleParams.comboLogLevels); i++)
		{
			bool is_selected = (consoleParams.current_log_filter_id == i);
			if (ImGui::Selectable(consoleParams.comboLogLevels[i], is_selected))
				consoleParams.current_log_filter_id = i;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void PerformanceMenu()
{
	ImGui::Begin("Performance");

	ImGui::Text("Frame render time: %.2f ms", performanceParams.timeToFrame * 1000);;

	performanceParams.lastFrames[performanceParams.frameIndex] = deltaTime;
	performanceParams.frameIndex++;

	if (performanceParams.frameIndex == 100)
	{
		performanceParams.frameIndex = 0;
		float frameTimeSum{};
		for (int i = 0; i < 100; i++)
		{
			frameTimeSum += performanceParams.lastFrames[i];
		}
		performanceParams.timeToFrame = frameTimeSum / 100;
	}

	ImGui::Text("Framerate: %i", (int)(1 / performanceParams.timeToFrame));

	if (performanceParams.isNvidiaGpu)
	{
		glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &performanceParams.total_mem_kb);
		glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &performanceParams.cur_avail_mem_kb);

		ImGui::Text("Total VRAM (MB): %i", performanceParams.total_mem_kb / 1000);
		ImGui::Text("Available VRAM (MB): %i", performanceParams.cur_avail_mem_kb / 1000);
		ImGui::Text("Used VRAM (MB): %i", (performanceParams.total_mem_kb - performanceParams.cur_avail_mem_kb) / 1000);
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

void ShaderSettings()
{
	ImGui::Begin("Shader settings");

	Shader* pbr = renderer->shaderManager.getShader(ShaderType::PBR);

	if (ImGui::DragFloat("Roughness", &shaderSettingsParams.roughness, 0.01f, 0.0f, 1.0f))
		pbr->use()->setFloat("material.roughness", shaderSettingsParams.roughness);

	if (ImGui::DragFloat("Metallic", &shaderSettingsParams.metallic, 0.01f, 0.0f, 1.0f))
		pbr->use()->setFloat("material.metallic", shaderSettingsParams.metallic);

	if (ImGui::DragFloat("AO", &shaderSettingsParams.ao, 0.01f, 0.0f, 1.0f))
		pbr->use()->setFloat("material.ao", shaderSettingsParams.ao);

	for (auto& [type, shader] : renderer->shaderManager.enumToShader)
	{
		std::string label;

		switch (type)
		{
			case ShaderType::PHONG:
				label = "Phong";
				break;

			case ShaderType::PBR:
				label = "PBR";
				break;

			case ShaderType::SKYBOX:
				label = "Skybox";
				break;

			case ShaderType::GRID:
				label = "Grid";
				break;

			case ShaderType::GRID2:
				label = "Grid2";
				break;

			case ShaderType::HDRTOCUBEMAP:
				label = "HDR to Cubemap";
				break;

			case ShaderType::IRRADIANCE:
				label = "IBL Irradiance";
				break;

			case ShaderType::PREFILTER:
				label = "IBL Prefilter";
				break;

			case ShaderType::BRDF:
				label = "BRDF";
				break;

			default:
				label = "Unnamed";
				break;
		}

		if (ImGui::CollapsingHeader(label.c_str()))
		{
			ImGui::PushID(shader->vertexPath.c_str());
			if (ImGui::Button("Edit vertex shader"))
			{
				shaderSettingsParams.currentEditedShaderPath = renderer->shaderManager.enumToShader[type]->vertexPath;
				shaderSettingsParams.currentEditedShaderType = type;
				shaderSettingsParams.isEditingVertexShader = true;
				shaderSettingsParams.isEditingShader = true;
				editor.SetText(renderer->shaderManager.getVertexShaderContent(type));
			}
			ImGui::PopID();

			ImGui::PushID(shader->fragmentPath.c_str());
			if (ImGui::Button("Edit fragment shader"))
			{
				shaderSettingsParams.currentEditedShaderPath = renderer->shaderManager.enumToShader[type]->fragmentPath;
				shaderSettingsParams.currentEditedShaderType = type;
				shaderSettingsParams.isEditingVertexShader = false;
				shaderSettingsParams.isEditingShader = true;
				editor.SetText(renderer->shaderManager.getFragmentShaderContent(type));
			}
			ImGui::PopID();
		}
	}

	ImGui::End();
}

void SetupEditor()
{
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();
	editor.SetLanguageDefinition(lang);
	editor.SetShowWhitespaces(false);
}

void ShowEditor()
{
	auto cpos = editor.GetCursorPosition();

	ImGui::Begin("Shader editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::Button("Save"))
		{
			auto textToSave = editor.GetText();
			if (shaderSettingsParams.isEditingShader)
			{
				if (shaderSettingsParams.isEditingVertexShader)
					renderer->shaderManager.setVertexShaderContent(shaderSettingsParams.currentEditedShaderType, textToSave);
				else
					renderer->shaderManager.setFragmentShaderContent(shaderSettingsParams.currentEditedShaderType, textToSave);

				renderer->shaderManager.enumToShader[shaderSettingsParams.currentEditedShaderType]->compileShader();
			}
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(), shaderSettingsParams.currentEditedShaderPath);

	editor.Render("TextEditor");
	ImGui::End();
}

void ShowNodeDetails()
{
	ImGui::Begin("Node details");

	if (selectedSceneNode != nullptr)
	{
		ImGui::Text(selectedSceneNode->getLabel().c_str());

		bool isVisible = selectedSceneNode->getIsEnabled();
		if (ImGui::Checkbox("Visible", &isVisible))
			selectedSceneNode->setIsEnabled(isVisible);

		for (auto& [type, component] : selectedSceneNode->getComponents())
			ShowComponentUI(component);
	}

	ImGui::End();
}

void SceneGraph()
{
	ImGui::Begin("Scene graph");

	if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
	{
		SceneGraphRecurse(renderer->GetEntities());
		ImGui::TreePop();
	}

	ImGui::End();
}

void SceneGraphRecurse(std::vector<Entity*> children)
{
	for (Entity* child : children)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;

		// Different flags for tree and leaf nodes
		if (child->getChildren().size() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;
		else
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;

		// Highlight selected node
		if (selectedSceneNode == child)
			flags |= ImGuiTreeNodeFlags_Selected;
		
		// Change text color for hidden nodes
		if (!child->getIsEnabled())
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
		
		if (!child->getIsEnabled())
			ImGui::PopStyleColor();
	}
}

void HandleSceneGraphClick(Entity* object)
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

			//renderer->removeObject(object);
			//delete object;
		}
			

		ImGui::EndPopup();
	}
}

void ShowComponentUI(Component* component)
{
	const std::type_index& componentType = typeid(component);

	if (dynamic_cast<TransformComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Transform"))
		{
			TransformComponent* transformComponent = dynamic_cast<TransformComponent*>(component);

			glm::vec3 position = transformComponent->getPosition();
			if (ImGui::DragFloat3("Position", &position[0]), 0.10f)
				transformComponent->setPosition(position);

			glm::vec3 rotation = transformComponent->getRotation();
			if (ImGui::DragFloat3("Rotation", &rotation[0]))
				transformComponent->setRotation(rotation);

			float scale = transformComponent->getScale()[0];
			if (ImGui::DragFloat("Scale", &scale, 0.001f))
				transformComponent->setScale(glm::vec3(scale));
		}
	}
	else if (dynamic_cast<SkyboxComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Skybox"))
		{
			SkyboxComponent* skyboxComponent = dynamic_cast<SkyboxComponent*>(component);

			const char* combo_preview_value = skyboxParams.comboSkyboxes[skyboxParams.current_skybox_id];
			if (ImGui::BeginCombo("##skySelector", combo_preview_value))
			{
				for (int i = 0; i < 3; i++)
				{
					const bool is_selected = (skyboxParams.current_skybox_id == i);
					if (ImGui::Selectable(skyboxParams.comboSkyboxes[i], is_selected))
					{
						skyboxParams.current_skybox_id = i;

						switch (skyboxParams.current_skybox_id)
						{
							case (int)SkyboxType::GRASS:
								skyboxComponent->changeSkybox(SkyboxType::GRASS);
								break;

							case (int)SkyboxType::NIGHT:
								skyboxComponent->changeSkybox(SkyboxType::NIGHT);
								break;

							case (int)SkyboxType::SKY:
								skyboxComponent->changeSkybox(SkyboxType::SKY);
								break;
						}
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
	}
	else if (dynamic_cast<MeshComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Mesh"))
		{
			MeshComponent* meshComponent = dynamic_cast<MeshComponent*>(component);
			
			PhongMaterial* phongMaterial = dynamic_cast<PhongMaterial*>(meshComponent->material);
			PBRMaterial* pbrMaterial = dynamic_cast<PBRMaterial*>(meshComponent->material);

			if (phongMaterial != nullptr)
			{
				ImGui::Text("Phong material:");

				ImGui::DragFloat("Shininess", &phongMaterial->shininess);

				ImGui::ColorEdit3("Ambient", &phongMaterial->ambientColor[0]);

				if (!phongMaterial->useDiffuseMap)
					ImGui::ColorEdit3("Diffuse", &phongMaterial->diffuseColor[0]);

				if (!phongMaterial->useSpecularMap)
					ImGui::ColorEdit3("Specular", &phongMaterial->specularColor[0]);
			}
			else if (pbrMaterial != nullptr)
			{
				ImGui::Text("PBR Material:");

				if (pbrMaterial->useAlbedoMap)
				{
					ImGui::Text("Albedo");

					float width = 128;
					float height = pbrMaterial->albedoTexture->height / pbrMaterial->albedoTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->albedoTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View albedo"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->albedoTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}
				else
					ImGui::ColorEdit3("Albedo color:", &pbrMaterial->albedoColor[0]);

				if (pbrMaterial->useNormalMap)
				{
					ImGui::Text("Normal map");

					float width = 128;
					float height = pbrMaterial->normalTexture->height / pbrMaterial->normalTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->normalTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View normal map"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->normalTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}

				if (pbrMaterial->useMetallicMap)
				{
					ImGui::Text("Metallic map");

					float width = 128;
					float height = pbrMaterial->metallicTexture->height / pbrMaterial->metallicTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->metallicTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View metallic map"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->metallicTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}
				else
					ImGui::DragFloat("Metallic:", &pbrMaterial->metallic, 0.01f, 0.0f, 1.0f);

				if (pbrMaterial->useRoughnessMap)
				{
					ImGui::Text("Roughness map");

					float width = 128;
					float height = pbrMaterial->roughnessTexture->height / pbrMaterial->roughnessTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->roughnessTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View roughness map"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->roughnessTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}
				else
					ImGui::DragFloat("Roughness:", &pbrMaterial->roughness, 0.01f, 0.0f, 1.0f);

				if (pbrMaterial->useAoMap)
				{
					ImGui::Text("Ambient occlusion map");

					float width = 128;
					float height = pbrMaterial->aoTexture->height / pbrMaterial->aoTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->aoTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View ambient occlusion map"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->aoTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}
				else
					ImGui::DragFloat("Ambient occlusion:", &pbrMaterial->ao, 0.01f, 0.0f, 1.0f);

				if (pbrMaterial->useOpacityMap)
				{
					ImGui::Text("Opacity map");

					float width = 128;
					float height = pbrMaterial->opacityTexture->height / pbrMaterial->opacityTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->opacityTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View opacity map"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->opacityTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}
				else
					ImGui::DragFloat("Opacity:", &pbrMaterial->opacity, 0.01f, 0.0f, 1.0f);

				if (pbrMaterial->useEmissiveMap)
				{
					ImGui::Text("Emissive map");

					float width = 128;
					float height = pbrMaterial->emissiveTexture->height / pbrMaterial->emissiveTexture->width * width;
					ImGui::Image((ImTextureID)pbrMaterial->emissiveTexture->texID, ImVec2(width, height));

					if (ImGui::Button("View emissive map"))
					{
						textureViewerParams.showTexture = true;
						textureViewerParams.currentTexture = pbrMaterial->emissiveTexture;
						ImGui::SetWindowFocus("Texture viewer");
					}
				}
			}
		}
	}
	else if (dynamic_cast<CameraComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Camera"))
		{
			CameraComponent* cameraComponent = dynamic_cast<CameraComponent*>(component);

			float currentZoom = cameraComponent->getZoom();
			float newZoom = currentZoom;

			// Camera FOV value
			ImGui::PushItemWidth(100.0f);
			ImGui::SliderFloat("FOV", &newZoom, 1.0f, 90.0f);

			if (newZoom != currentZoom)
				cameraComponent->setZoom(newZoom);

			float currentSpeed = cameraComponent->getSpeed();
			float newSpeed = currentSpeed;

			float currentSensitivity = cameraComponent->getSensitivity();
			float newSensitivity = currentSensitivity;

			// Camera speed and sensitivity
			ImGui::PushItemWidth(100.0f);
			ImGui::InputFloat("Speed", &newSpeed);
			ImGui::InputFloat("Sensitivity", &newSensitivity);

			if (newSpeed != currentSpeed)
				cameraComponent->setSpeed(newSpeed);

			if (newSensitivity != currentSensitivity)
				cameraComponent->setSensitivity(newSensitivity);

			if (ImGui::Button("Reset"))
			{
				cameraComponent->setZoom(CameraComponent::ZOOM);
				cameraComponent->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				cameraComponent->setSpeed(CameraComponent::SPEED);
				cameraComponent->setSensitivity(CameraComponent::SENSITIVITY);
			}

			ImGui::PopItemWidth();
			ImGui::PopItemWidth();
		}
	}
	else if (dynamic_cast<PointLightComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Point light"))
		{
			PointLightComponent* pointLightComponent = dynamic_cast<PointLightComponent*>(component);

			ImGui::ColorEdit3("Ambient", &(pointLightComponent->ambientColor[0]));
			ImGui::ColorEdit3("Diffuse", &(pointLightComponent->diffuseColor[0]));
			ImGui::ColorEdit3("Specular", &(pointLightComponent->specularColor[0]));

			ImGui::DragFloat("Constant", &(pointLightComponent->constant), 0.002f, 0.0f, 1.0f);
			ImGui::DragFloat("Linear", &(pointLightComponent->linear), 0.002f, 0.0f, 1.0f);
			ImGui::DragFloat("Quadratic", &(pointLightComponent->quadratic), 0.002f, 0.001f, 1.0f);
		}
	}
	else if (dynamic_cast<SpotLightComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Spot light"))
		{
			SpotLightComponent* spotLightComponent = dynamic_cast<SpotLightComponent*>(component);

			ImGui::ColorEdit3("Ambient", &(spotLightComponent->ambientColor[0]));
			ImGui::ColorEdit3("Diffuse", &(spotLightComponent->diffuseColor[0]));
			ImGui::ColorEdit3("Specular", &(spotLightComponent->specularColor[0]));

			ImGui::DragFloat("Constant", &(spotLightComponent->constant), 0.002f, 0.0f, 1.0f);
			ImGui::DragFloat("Linear", &(spotLightComponent->linear), 0.002f, 0.0f, 1.0f);
			ImGui::DragFloat("Quadratic", &(spotLightComponent->quadratic), 0.002f, 0.001f, 1.0f);

			ImGui::DragFloat("Cutoff", &(spotLightComponent->cutOff), 0.002f, 0.001f, 1.0f);
			ImGui::DragFloat("Outer cutoff", &(spotLightComponent->outerCutOff), 0.002f, 0.001f, 1.0f);

			ImGui::DragFloat3("Direction", &(spotLightComponent->direction[0]), 0.002f);
		}
	}
	else if (dynamic_cast<DirectionalLightComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Directional light"))
		{
			DirectionalLightComponent* directionalLightComponent = dynamic_cast<DirectionalLightComponent*>(component);

			ImGui::ColorEdit3("Ambient", &(directionalLightComponent->ambientColor[0]));
			ImGui::ColorEdit3("Diffuse", &(directionalLightComponent->diffuseColor[0]));
			ImGui::ColorEdit3("Specular", &(directionalLightComponent->specularColor[0]));

			ImGui::DragFloat3("Direction", &(directionalLightComponent->direction[0]), 0.002f);
		}
	}
}

void TextureViewer()
{
	ImGui::Begin("Texture viewer");

	if (textureViewerParams.showTexture)
	{
		ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();

		// We want the image to always fit in the space of the window
		float width = contentRegionSize.x;
		float height = textureViewerParams.currentTexture->height / textureViewerParams.currentTexture->width * width;
		ImGui::Image((ImTextureID)textureViewerParams.currentTexture->texID, ImVec2(width, height));

		std::string dimensions = "Size: " + std::to_string((int)textureViewerParams.currentTexture->width) + "x" + std::to_string((int)textureViewerParams.currentTexture->height) + " px";
		ImGui::Text(dimensions.c_str());
	}

	ImGui::End();
}