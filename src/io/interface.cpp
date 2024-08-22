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
#include "components/scriptComponent.hpp"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

// Used for dynamically showing existing lights and enabling their realtime modification
Renderer* renderer;

// The currently selected node in the scene graph
Entity* selectedSceneNode{};

std::string editLabel{};

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
	// The corresponding enums
	LogLevel logLevelsEnums[4] = { LogLevel::LOG_INFO, LogLevel::LOG_WARNING, LogLevel::LOG_ERROR, LogLevel::LOG_DEBUG };

	// The colors for each log level
	std::map<LogLevel, ImVec4> levelToColor =
	{
		{ LogLevel::LOG_INFO, ImVec4(0.8f, 0.8f, 0.8f, 1.0f) },
		{ LogLevel::LOG_WARNING, ImVec4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ LogLevel::LOG_ERROR, ImVec4(1.0f, 0.1f, 0.1f, 1.0f) },
		{ LogLevel::LOG_DEBUG, ImVec4(0.1f, 0.1f, 1.0f, 1.0f) },
	};

	// The labels for the log levels
	std::map<LogLevel, const char*> levelToLabel = {
		{ LogLevel::LOG_INFO, "Info" },
		{ LogLevel::LOG_WARNING, "Warning" },
		{ LogLevel::LOG_ERROR, "Error" },
		{ LogLevel::LOG_DEBUG, "Debug" },
	};

	// The toggles for the checkboxes
	std::map<LogLevel, bool> logLevelsToggles = {
		{ LogLevel::LOG_INFO, true },
		{ LogLevel::LOG_WARNING, true },
		{ LogLevel::LOG_ERROR, true },
		{ LogLevel::LOG_DEBUG, true },
	};

	// Currently selected log levels
	std::set<LogLevel> selectedLogLevels = { LogLevel::LOG_INFO, LogLevel::LOG_WARNING, LogLevel::LOG_ERROR, LogLevel::LOG_DEBUG };

	// Contains whether a source file's logs should be shown or not
	std::map<std::string, bool> sourceFileToggle;

	// Currently selected source files
	std::set<std::string> selectedSourceFiles;

	// A cached vector containing the current logs shown in the console
	std::vector<Log> shownLogs;

	// How many logs the logger contained when it was last filtered, this is used to implement a caching system to improve performance
	int lastFilteredVectorSize = 0;
} consoleParams;

struct
{
	// For the shader editor
	TextEditor editor;

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
	// For the script editor
	TextEditor editor;

	ScriptComponent* currentEditedScript = nullptr;
	std::string currrentEditedScriptName = "";
	bool isEditingScript = false;
} scriptEditorParams;

struct
{
	std::shared_ptr<Texture> currentTexture = nullptr;
	bool showTexture = false;
} textureViewerParams;


void ImGuiInit(GLFWwindow* window, Renderer* rendererArg)
{
	// OpenGL context needs to be initalized beforehand to call glGetString()
	performanceParams.gpuVendor = (char*)glGetString(GL_VENDOR);
	performanceParams.gpuVendorStr = std::string(performanceParams.gpuVendor);
	performanceParams.isNvidiaGpu = performanceParams.gpuVendorStr.find("NVIDIA") != std::string::npos;

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

	SetupShaderEditor();
	SetupScriptEditor();

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
	ShowShaderEditor();
	ShowScriptEditor();
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

	ImVec2 viewerSize = ImGui::GetContentRegionAvail();

	defaultRenderer.resizeFramebuffers(glm::vec2(viewerSize.x, viewerSize.y));

	// Since we are about to draw the image, this returns the top left corner of the image
	glm::vec2 imagePos = ImGui::GetCursorScreenPos();

	ImGui::Image(
		(ImTextureID)renderer->getRenderTexture(),
		viewerSize,
		ImVec2(0, 1),
		ImVec2(1, 0)
	);

	isViewerFocused = ImGui::IsWindowFocused();

	// If we have a click inside the viewer window
	if (isViewerFocused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		glm::vec2 mousePos = ImGui::GetMousePos();
		// Get mouse position inside of the viewer
		glm::vec2 relativeMousePos = mousePos - imagePos;

		// Inspired from https://www.mvps.org/directx/articles/rayproj.htm
		
		// Calculate X click position on range -1 to 1
		float ndcX = relativeMousePos.x / viewerSize.x * 2.0f - 1.0f;
		// Calculate Y click position on range -1 to 1
		float ndcY = 1.0f - relativeMousePos.y / viewerSize.y * 2.0f;

		// This is a click inside of the window
		if (ndcX > -1.0f && ndcX < 1.0f && ndcY > -1.0f && ndcY < 1.0f)
		{
			std::string hitMessage = std::to_string(ndcX) + " - " + std::to_string(ndcY);
			Logger::logInfo(hitMessage, "interface.cpp");
			
			float cameraFov = glm::radians(cameraComponent->getZoom());
			float aspectRatio = viewerSize.x / viewerSize.y;

			float dx = tanf(cameraFov * 0.5f) * ndcX * aspectRatio;
			float dy = tanf(cameraFov * 0.5f) * ndcY;

			glm::vec4 rayStartPosView = glm::vec4(dx * CameraComponent::NEAR, dy * CameraComponent::NEAR, -CameraComponent::NEAR, 1.0f);
			glm::vec4 rayEndPosView = glm::vec4(dx * CameraComponent::FAR, dy * CameraComponent::FAR, -CameraComponent::FAR, 1.0f);

			glm::mat4 cameraViewInv = glm::inverse(cameraComponent->getViewMatrix());

			glm::vec3 rayStartPosWorld = cameraViewInv * rayStartPosView;
			glm::vec3 rayEndPosWorld = cameraViewInv * rayEndPosView;

			//defaultRenderer.addLine(rayStartPosWorld, rayEndPosWorld, true);
			PhysicsComponent* raycastResult = defaultRenderer.physicsWorld->raycastLine(rayStartPosWorld, rayEndPosWorld);

			if (selectedSceneNode != nullptr)
				selectedSceneNode->drawOutline = false;

			if (raycastResult != nullptr)
			{
				selectedSceneNode = raycastResult->parent;
				selectedSceneNode->drawOutline = true;
			}
			else
				selectedSceneNode = nullptr;
		}
	}

	ImGui::End();
}

void ShowConsole()
{
	ImGui::Begin("Console");

	ImVec2 windowSize = ImGui::GetContentRegionAvail();

	// We keep 20px at the bottom for the buttons
	ImGui::BeginChild("Logs", ImVec2(0, windowSize.y - 20), 0, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.2f, 0.2f, 0.5f));

	int currentLogCount = Logger::getLogCount();
	if (consoleParams.lastFilteredVectorSize != currentLogCount)
	{
		consoleParams.shownLogs = Logger::getFilteredLogs(consoleParams.selectedLogLevels, consoleParams.selectedSourceFiles);
		consoleParams.lastFilteredVectorSize = currentLogCount;
	}

	// Display all the logs with their corresponding colors, with the earliest first
	for (int i = consoleParams.shownLogs.size() - 1; i > 0; i--)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, consoleParams.levelToColor[consoleParams.shownLogs[i].logLevel]);
		ImGui::Text(consoleParams.shownLogs[i].logMessage.c_str());
		ImGui::PopStyleColor();
	}

	ImGui::PopStyleColor();
	ImGui::EndChild();

	if (ImGui::Button("Clear logs"))
		Logger::clearLogs();

	ImGui::SameLine();
	ImGui::Text("Filters:");

	ImGui::PushItemWidth(100);
	
	ImGui::SameLine();
	if (ImGui::BeginCombo("##logLevelsFilterCombo", "Levels"))
	{
		for (LogLevel level : consoleParams.logLevelsEnums)
		{
			if (ImGui::Checkbox(consoleParams.levelToLabel[level], &consoleParams.logLevelsToggles[level]))
			{
				if (consoleParams.logLevelsToggles[level])
					consoleParams.selectedLogLevels.insert(level);
				else
					consoleParams.selectedLogLevels.erase(level);

				consoleParams.shownLogs = Logger::getFilteredLogs(consoleParams.selectedLogLevels, consoleParams.selectedSourceFiles);
			}
		}

		ImGui::EndCombo();
	}

	std::vector<std::string> sourceFiles = Logger::getSourceFiles();

	// Refresh if there are any new source files
	for (std::string file : sourceFiles)
	{
		if (consoleParams.sourceFileToggle.count(file) == 0)
		{
			// Any new source file starts as toggled on
			consoleParams.selectedSourceFiles.insert(file);
			consoleParams.sourceFileToggle[file] = true;
		}
	}

	ImGui::SameLine();
	if (ImGui::BeginCombo("##logFilesFilterCombo", "Sources"))
	{
		for (std::string file : sourceFiles)
		{
			if (consoleParams.sourceFileToggle.count(file) == 0)
			{
				// Any new source file starts as toggled on
				consoleParams.selectedSourceFiles.insert(file);
				consoleParams.sourceFileToggle[file] = true;
			}

			if (ImGui::Checkbox(file.c_str(), &consoleParams.sourceFileToggle[file]))
			{
				if (consoleParams.sourceFileToggle[file])
					consoleParams.selectedSourceFiles.insert(file);
				else
					consoleParams.selectedSourceFiles.erase(file);

				consoleParams.shownLogs = Logger::getFilteredLogs(consoleParams.selectedLogLevels, consoleParams.selectedSourceFiles);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::PopItemWidth();
	ImGui::End();
}

void PerformanceMenu()
{
	ImGui::Begin("Performance");

	ImGui::Text("Frame render time: %.2f ms", deltaTime * 1000);
	ImGui::Text("Mesh sorting time: %.2f ms", renderer->meshSortingTime * 1000);
	ImGui::Text("Physics update time: %.2f ms", renderer->physicsUpdateTime * 1000);
	ImGui::Text("Shadow pass time: %.2f ms", renderer->shadowPassTime * 1000);
	ImGui::Text("gBuffer pass time: %.2f ms", renderer->gBufferPassTime * 1000);
	ImGui::Text("SSAO pass time: %.2f ms", renderer->ssaoPassTime * 1000);
	ImGui::Text("Render pass time: %.2f ms", renderer->renderPassTime * 1000);
	ImGui::Text("Outline pass time: %.2f ms", renderer->outlinePassTime * 1000);
	ImGui::Text("Blit pass time: %.2f ms", renderer->blitPassTime * 1000);

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

	ImGui::Separator();

	ImGui::Text("Mesh sorting time: %.2f ms", renderer->meshSortingTime * 1000);
	ImGui::Text("Physics update time: %.2f ms", renderer->physicsUpdateTime * 1000);
	ImGui::Text("Shadow pass time: %.2f ms", renderer->shadowPassTime * 1000);
	ImGui::Text("gBuffer pass time: %.2f ms", renderer->gBufferPassTime * 1000);
	ImGui::Text("SSAO pass time: %.2f ms", renderer->ssaoPassTime * 1000);
	ImGui::Text("Render pass time: %.2f ms", renderer->renderPassTime * 1000);
	ImGui::Text("Outline pass time: %.2f ms", renderer->outlinePassTime * 1000);
	ImGui::Text("Blit pass time: %.2f ms", renderer->blitPassTime * 1000);

	if (performanceParams.isNvidiaGpu)
	{
		ImGui::Separator();

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

			case ShaderType::SOLID:
				label = "Solid";
				break;

			case ShaderType::OUTLINE:
				label = "Outline";
				break;

			case ShaderType::DEPTH:
				label = "Depth";
				break;

			case ShaderType::GBUFFER:
				label = "G Buffer";
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
				shaderSettingsParams.editor.SetText(renderer->shaderManager.getVertexShaderContent(type));
				ImGui::SetWindowFocus("Shader editor");
			}
			ImGui::PopID();

			ImGui::PushID(shader->fragmentPath.c_str());
			if (ImGui::Button("Edit fragment shader"))
			{
				shaderSettingsParams.currentEditedShaderPath = renderer->shaderManager.enumToShader[type]->fragmentPath;
				shaderSettingsParams.currentEditedShaderType = type;
				shaderSettingsParams.isEditingVertexShader = false;
				shaderSettingsParams.isEditingShader = true;
				shaderSettingsParams.editor.SetText(renderer->shaderManager.getFragmentShaderContent(type));
				ImGui::SetWindowFocus("Shader editor");
			}
			ImGui::PopID();
		}
	}

	ImGui::End();
}

void SetupShaderEditor()
{
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::GLSL();
	shaderSettingsParams.editor.SetLanguageDefinition(lang);
	shaderSettingsParams.editor.SetShowWhitespaces(false);
}

void ShowShaderEditor()
{
	auto cpos = shaderSettingsParams.editor.GetCursorPosition();

	ImGui::Begin("Shader editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::Button("Save"))
		{
			auto textToSave = shaderSettingsParams.editor.GetText();
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
			bool ro = shaderSettingsParams.editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				shaderSettingsParams.editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && shaderSettingsParams.editor.CanUndo()))
				shaderSettingsParams.editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && shaderSettingsParams.editor.CanRedo()))
				shaderSettingsParams.editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, shaderSettingsParams.editor.HasSelection()))
				shaderSettingsParams.editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && shaderSettingsParams.editor.HasSelection()))
				shaderSettingsParams.editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && shaderSettingsParams.editor.HasSelection()))
				shaderSettingsParams.editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				shaderSettingsParams.editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				shaderSettingsParams.editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(shaderSettingsParams.editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				shaderSettingsParams.editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				shaderSettingsParams.editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				shaderSettingsParams.editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, shaderSettingsParams.editor.GetTotalLines(),
		shaderSettingsParams.editor.IsOverwrite() ? "Ovr" : "Ins",
		shaderSettingsParams.editor.CanUndo() ? "*" : " ",
		shaderSettingsParams.editor.GetLanguageDefinition().mName.c_str(), shaderSettingsParams.currentEditedShaderPath);

	shaderSettingsParams.editor.Render("Shader Editor");
	ImGui::End();
}

void SetupScriptEditor()
{
	TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::Lua();
	scriptEditorParams.editor.SetLanguageDefinition(lang);
	scriptEditorParams.editor.SetShowWhitespaces(false);
}

void ShowScriptEditor()
{
	auto cpos = scriptEditorParams.editor.GetCursorPosition();

	ImGui::Begin("Script editor", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::Button("Save"))
		{
			auto textToSave = scriptEditorParams.editor.GetText();
			if (scriptEditorParams.isEditingScript)
				scriptEditorParams.currentEditedScript->scriptCode = textToSave;
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = scriptEditorParams.editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				scriptEditorParams.editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && scriptEditorParams.editor.CanUndo()))
				scriptEditorParams.editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && scriptEditorParams.editor.CanRedo()))
				scriptEditorParams.editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, scriptEditorParams.editor.HasSelection()))
				scriptEditorParams.editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && scriptEditorParams.editor.HasSelection()))
				scriptEditorParams.editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && scriptEditorParams.editor.HasSelection()))
				scriptEditorParams.editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				scriptEditorParams.editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				scriptEditorParams.editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(scriptEditorParams.editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				scriptEditorParams.editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				scriptEditorParams.editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				scriptEditorParams.editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, scriptEditorParams.editor.GetTotalLines(),
		scriptEditorParams.editor.IsOverwrite() ? "Ovr" : "Ins",
		scriptEditorParams.editor.CanUndo() ? "*" : " ",
		scriptEditorParams.editor.GetLanguageDefinition().mName.c_str(), scriptEditorParams.currrentEditedScriptName);

	scriptEditorParams.editor.Render("Script Editor");
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
		SceneGraphRecurse(renderer->getEntities());
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
	{
		if (selectedSceneNode != nullptr) selectedSceneNode->drawOutline = false;
		selectedSceneNode = object;
		selectedSceneNode->drawOutline = true;
	}
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

			if (!renderer->removeEntity(object))
			{
				object->getParent()->removeChild(object);
				delete object;
			}
		}

		ImGui::EndPopup();
	}
}

void ShowComponentUI(Component* component)
{
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

			std::string verticesText = "Vertices: " + std::to_string(meshComponent->getVerticesCount());
			std::string indicesText = "Indices: " + std::to_string(meshComponent->getIndicesCount());

			ImGui::Text(verticesText.c_str());
			ImGui::Text(indicesText.c_str());

			PhongMaterial* phongMaterial = dynamic_cast<PhongMaterial*>(meshComponent->material.get());
			PBRMaterial* pbrMaterial = dynamic_cast<PBRMaterial*>(meshComponent->material.get());

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
	else if (dynamic_cast<ScriptComponent*>(component))
	{
		if (ImGui::CollapsingHeader("Script"))
		{
			ScriptComponent* scriptComponent = dynamic_cast<ScriptComponent*>(component);

			if (ImGui::Button("Edit script"))
			{
				scriptEditorParams.currentEditedScript = scriptComponent;
				scriptEditorParams.isEditingScript = true;
				scriptEditorParams.editor.SetText(scriptComponent->scriptCode);
			}
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