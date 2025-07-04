#include <string>
#include <vector>

#include "ImGui/imgui.h"
#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGuiColorTextEdit/TextEditor.h"

#include "main.hpp"
#include "entity.hpp"
#include "io/input.hpp"
#include "io/interface.hpp"
#include "lightManager.hpp"
#include "logger.hpp"
#include "game/gameEngine.hpp"
#include "game/gameState.hpp"
#include "materials/pbrMaterial.hpp"
#include "materials/phongMaterial.hpp"

#include "components/meshComponent.hpp"
#include "components/transformComponent.hpp"
#include "components/cameraComponent.hpp"
#include "components/lights/lightComponent.hpp"
#include "components/lights/pointLightComponent.hpp"
#include "components/lights/spotLightComponent.hpp"
#include "components/lights/directionalLightComponent.hpp"
#include "components/skyboxComponent.hpp"
#include "components/scriptComponent.hpp"

namespace Interface
{
	std::string editLabel{};

	bool isViewerFocused = false;

	float interfaceDrawTime = 0.0f;

	struct
	{
		bool shaderEditorOn = false;
		bool scriptEditorOn = false;
		bool textureViewerOn = false;
		bool topViewOn = false;
	} toolbarParams;

	struct
	{
		// An array containing the choice names for the different skyboxes
		const char* comboSkyboxes[3] = { "Grass", "Night", "Sky" };
		int currentSkyboxId = static_cast<int>(SkyboxComponent::DEFAULT_SKY);
	} skyboxParams;

	struct PerformanceParams
	{
		static constexpr int GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX = 0x9048;
		static constexpr int GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX = 0x9049;

		// For calculating the FPS, the time taken to render the last 100 frames is used
		float lastFrames[100]{};
		int frameIndex{};
		float timeToFrame = 1;

		// NVIDIA GPU only, serves to query the amount of total and available VRAM
		int totalMemoryKb = 0;
		int currentAvailableMemoryKb = 0;
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
		std::string currrentEditedScriptName;
		bool isEditingScript = false;
	} scriptEditorParams;

	struct
	{
		std::shared_ptr<Texture> currentTexture = nullptr;
		bool showTexture = false;
	} textureViewerParams;


	void ImGuiInit(GLFWwindow* window)
	{
		// OpenGL context needs to be initalized beforehand to call glGetString()
		performanceParams.gpuVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
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
	}

	void ImGuiDrawWindows()
	{
		float startTime = glfwGetTime();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::BeginMainMenuBar();
		ImVec2 mainMenuBarSize = ImGui::GetWindowSize();
		
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Shader editor", nullptr, &toolbarParams.shaderEditorOn);
			ImGui::MenuItem("Script editor", nullptr, &toolbarParams.scriptEditorOn);
			ImGui::MenuItem("Texture viewer", nullptr, &toolbarParams.textureViewerOn);
			ImGui::MenuItem("Top view", nullptr, &toolbarParams.topViewOn);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		// We want to create a full size window
		glm::vec2 windowSize = Input::getWindowSize();

		ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarSize.y));
		ImGui::SetNextWindowSize(ImVec2(windowSize.x, windowSize.y - mainMenuBarSize.y));
		ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

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

		interfaceDrawTime = glfwGetTime() - startTime;
	}

	void ShowViewer()
	{
		ImGui::Begin("Viewer");

		ImVec2 viewerSize = ImGui::GetContentRegionAvail();

		Main::game.renderer.resizeFramebuffers(glm::vec2(viewerSize.x, viewerSize.y));

		// Since we are about to draw the image, this returns the top left corner of the image
		glm::vec2 imagePos = ImGui::GetCursorScreenPos();

		ImGui::Image(
			reinterpret_cast<ImTextureID>(Main::game.renderer.getRenderTexture()),
			viewerSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		isViewerFocused = ImGui::IsWindowFocused();

		// If we have a click inside the viewer window
		if (true || isViewerFocused && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			glm::vec2 mousePos = ImGui::GetMousePos();
			// Get mouse position inside the viewer
			glm::vec2 relativeMousePos = mousePos - imagePos;

			// Inspired from https://www.mvps.org/directx/articles/rayproj.htm

			// Calculate X click position on range -1 to 1
			float ndcX = relativeMousePos.x / viewerSize.x * 2.0f - 1.0f;
			// Calculate Y click position on range -1 to 1
			float ndcY = 1.0f - relativeMousePos.y / viewerSize.y * 2.0f;

			// This is a click inside the window
			if (ndcX > -1.0f && ndcX < 1.0f && ndcY > -1.0f && ndcY < 1.0f)
			{
				std::string hitMessage = std::to_string(ndcX) + " - " + std::to_string(ndcY);
				Logger::logInfo(hitMessage, "interface.cpp");

				float cameraFov = glm::radians(Main::game.getCurrentState()->getScene().currentCamera->getZoom());
				float aspectRatio = viewerSize.x / viewerSize.y;

				float dx = tanf(cameraFov * 0.5f) * ndcX * aspectRatio;
				float dy = tanf(cameraFov * 0.5f) * ndcY;

				glm::vec4 rayStartPosView = glm::vec4(dx * CameraComponent::NEAR, dy * CameraComponent::NEAR, -CameraComponent::NEAR, 1.0f);
				glm::vec4 rayEndPosView = glm::vec4(dx * CameraComponent::FAR, dy * CameraComponent::FAR, -CameraComponent::FAR, 1.0f);

				glm::mat4 cameraViewInv = glm::inverse(Main::game.getCurrentState()->getScene().currentCamera->getViewMatrix());

				glm::vec3 rayStartPosWorld = cameraViewInv * rayStartPosView;
				glm::vec3 rayEndPosWorld = cameraViewInv * rayEndPosView;

				//defaultRenderer.addLine(rayStartPosWorld, rayEndPosWorld, true);
				PhysicsComponent* raycastResult = Main::game.getCurrentState()->getPhysicsWorld().raycastLine(rayStartPosWorld, rayEndPosWorld);

				if (Main::game.getCurrentState()->getScene().currentActiveEntity != nullptr)
					Main::game.getCurrentState()->getScene().currentActiveEntity->drawOutline = false;

				if (raycastResult != nullptr)
				{
					Main::game.getCurrentState()->getScene().currentActiveEntity = raycastResult->parent;
					Main::game.getCurrentState()->getScene().currentActiveEntity->drawOutline = true;
				}
				else
					Main::game.getCurrentState()->getScene().currentActiveEntity = nullptr;
			}
		}

		ImGui::End();

		if (!toolbarParams.topViewOn)
			return;
		
		// Second camera view
		ImGui::Begin("Top view");

		ImGui::Image(
			reinterpret_cast<ImTextureID>(Main::game.renderer.getSkyRenderTexture()),
			viewerSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		ImGui::End();
	}

	void ShowConsole()
	{
		ImGui::Begin("Console");

		ImVec2 lastWindowSize = ImGui::GetContentRegionAvail();

		std::vector<std::string> sourceFiles = Logger::getSourceFiles();

		// Refresh if there are any new source files
		for (const std::string& file : sourceFiles)
		{
			if (consoleParams.sourceFileToggle.count(file) == 0)
			{
				// Any new source file starts as toggled on
				consoleParams.selectedSourceFiles.insert(file);
				consoleParams.sourceFileToggle[file] = true;
			}
		}

		// We keep 20px at the bottom for the buttons
		ImGui::BeginChild("Logs", ImVec2(0, lastWindowSize.y - 20), 0, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.2f, 0.2f, 0.5f));

		int currentLogCount = Logger::getLogCount();
		if (consoleParams.lastFilteredVectorSize != currentLogCount)
		{
			consoleParams.shownLogs = Logger::getFilteredLogs(consoleParams.selectedLogLevels, consoleParams.selectedSourceFiles);
			consoleParams.lastFilteredVectorSize = currentLogCount;
		}

		// Display all the logs with their corresponding colors, with the earliest first
		for (int i = consoleParams.shownLogs.size() - 1; i >= 0; i--)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, consoleParams.levelToColor[consoleParams.shownLogs[i].logLevel]);
			Log log = consoleParams.shownLogs[i];

			if (log.logCount > 1) // We display the log count as well for repeated logs
				ImGui::Text("%s", (log.logMessage + " (x" + std::to_string(log.logCount) + ")").c_str());
			else // Normal display if log was not repeated
				ImGui::Text("%s", consoleParams.shownLogs[i].logMessage.c_str());

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

		ImGui::SameLine();
		if (ImGui::BeginCombo("##logFilesFilterCombo", "Sources"))
		{
			for (const std::string& file : sourceFiles)
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
		Renderer& renderer = Main::game.renderer;

		ImGui::Begin("Performance");

		ImGui::Text("Total frame time: %.2f ms", Main::deltaTime * 1000);
		ImGui::Text("Render time: %.2f ms", renderer.frameRenderTime * 1000);
		ImGui::Text("UI Draw time: %.2f ms", interfaceDrawTime * 1000);

		performanceParams.lastFrames[performanceParams.frameIndex] = Main::deltaTime;
		performanceParams.frameIndex++;

		if (performanceParams.frameIndex == 100)
		{
			performanceParams.frameIndex = 0;
			float frameTimeSum{};
			for (float lastFrame : performanceParams.lastFrames)
				frameTimeSum += lastFrame;

			performanceParams.timeToFrame = frameTimeSum / 100;
		}

		ImGui::Text("Framerate: %i", static_cast<int>(1 / performanceParams.timeToFrame));

		ImGui::Separator();

		ImGui::Text("Mesh sorting time: %.2f ms", renderer.meshSortingTime * 1000);
		ImGui::Text("Physics update time: %.2f ms", renderer.physicsUpdateTime * 1000);
		ImGui::Text("Shadow pass time: %.2f ms", renderer.shadowPassTime * 1000);
		ImGui::Text("gBuffer pass time: %.2f ms", renderer.gBufferPassTime * 1000);
		ImGui::Text("SSAO pass time: %.2f ms", renderer.ssaoPassTime * 1000);
		ImGui::Text("Render pass time: %.2f ms", renderer.renderPassTime * 1000);
		ImGui::Text("Outline pass time: %.2f ms", renderer.outlinePassTime * 1000);
		ImGui::Text("Blit pass time: %.2f ms", renderer.blitPassTime * 1000);
		ImGui::Text("Debug pass time: %.2f ms", renderer.debugPassTime * 1000);

		if (performanceParams.isNvidiaGpu)
		{
			ImGui::Separator();

			glGetIntegerv(Interface::PerformanceParams::GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &performanceParams.totalMemoryKb);
			glGetIntegerv(Interface::PerformanceParams::GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &performanceParams.currentAvailableMemoryKb);

			ImGui::Text("Total VRAM (MB): %i", performanceParams.totalMemoryKb / 1000);
			ImGui::Text("Available VRAM (MB): %i", performanceParams.currentAvailableMemoryKb / 1000);
			ImGui::Text("Used VRAM (MB): %i", (performanceParams.totalMemoryKb - performanceParams.currentAvailableMemoryKb) / 1000);
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

		Shader* pbr = Main::game.renderer.shaderManager.getShader(ShaderType::PBR);

		if (ImGui::DragFloat("Roughness", &shaderSettingsParams.roughness, 0.01f, 0.0f, 1.0f))
			pbr->use()->setFloat("material.roughness", shaderSettingsParams.roughness);

		if (ImGui::DragFloat("Metallic", &shaderSettingsParams.metallic, 0.01f, 0.0f, 1.0f))
			pbr->use()->setFloat("material.metallic", shaderSettingsParams.metallic);

		if (ImGui::DragFloat("AO", &shaderSettingsParams.ao, 0.01f, 0.0f, 1.0f))
			pbr->use()->setFloat("material.ao", shaderSettingsParams.ao);

		for (auto& [type, shader] : Main::game.renderer.shaderManager.enumToShader)
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
				Renderer& renderer = Main::game.renderer;

				ImGui::PushID(shader->vertexPath.c_str());
				if (ImGui::Button("Edit vertex shader"))
				{
					shaderSettingsParams.currentEditedShaderPath = renderer.shaderManager.enumToShader[type]->vertexPath;
					shaderSettingsParams.currentEditedShaderType = type;
					shaderSettingsParams.isEditingVertexShader = true;
					shaderSettingsParams.isEditingShader = true;
					shaderSettingsParams.editor.SetText(renderer.shaderManager.getVertexShaderContent(type));

					toolbarParams.shaderEditorOn = true;
					ImGui::SetWindowFocus("Shader editor");
				}
				ImGui::PopID();

				ImGui::PushID(shader->fragmentPath.c_str());
				if (ImGui::Button("Edit fragment shader"))
				{
					shaderSettingsParams.currentEditedShaderPath = renderer.shaderManager.enumToShader[type]->fragmentPath;
					shaderSettingsParams.currentEditedShaderType = type;
					shaderSettingsParams.isEditingVertexShader = false;
					shaderSettingsParams.isEditingShader = true;
					shaderSettingsParams.editor.SetText(renderer.shaderManager.getFragmentShaderContent(type));

					toolbarParams.shaderEditorOn = true;
					ImGui::SetWindowFocus("Shader editor");
				}
				ImGui::PopID();
			}
		}

		ImGui::End();
	}

	void SetupShaderEditor()
	{
		const TextEditor::LanguageDefinition& lang = TextEditor::LanguageDefinition::GLSL();
		shaderSettingsParams.editor.SetLanguageDefinition(lang);
		shaderSettingsParams.editor.SetShowWhitespaces(false);
	}

	void ShowShaderEditor()
	{
		if (!toolbarParams.shaderEditorOn)
			return;

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
					Renderer& renderer = Main::game.renderer;
					if (shaderSettingsParams.isEditingVertexShader)
						renderer.shaderManager.setVertexShaderContent(shaderSettingsParams.currentEditedShaderType, textToSave);
					else
						renderer.shaderManager.setFragmentShaderContent(shaderSettingsParams.currentEditedShaderType, textToSave);

					renderer.shaderManager.enumToShader[shaderSettingsParams.currentEditedShaderType]->compileShader();
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
			shaderSettingsParams.editor.GetLanguageDefinition().mName.c_str(),
			shaderSettingsParams.currentEditedShaderPath.c_str());

		shaderSettingsParams.editor.Render("Shader Editor");
		ImGui::End();
	}

	void SetupScriptEditor()
	{
		const TextEditor::LanguageDefinition& lang = TextEditor::LanguageDefinition::Lua();
		scriptEditorParams.editor.SetLanguageDefinition(lang);
		scriptEditorParams.editor.SetShowWhitespaces(false);
	}

	void ShowScriptEditor()
	{
		if (!toolbarParams.scriptEditorOn)
			return;

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
			scriptEditorParams.editor.GetLanguageDefinition().mName.c_str(),
			scriptEditorParams.currrentEditedScriptName.c_str());

		scriptEditorParams.editor.Render("Script Editor");
		ImGui::End();
	}

	void ShowNodeDetails()
	{
		ImGui::Begin("Node details");

		if (Main::game.getCurrentState()->getScene().currentActiveEntity != nullptr)
		{

			ImGui::Text("%s", Main::game.getCurrentState()->getScene().currentActiveEntity->getLabel().c_str());

			bool isVisible = Main::game.getCurrentState()->getScene().currentActiveEntity->getIsEnabled();
			if (ImGui::Checkbox("Visible", &isVisible))
				Main::game.getCurrentState()->getScene().currentActiveEntity->setIsEnabled(isVisible);

			for (auto& [type, component] : Main::game.getCurrentState()->getScene().currentActiveEntity->getComponents())
				ShowComponentUI(component);
		}

		ImGui::End();
	}

	void SceneGraph()
	{
		ImGui::Begin("Scene graph");

		if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			// TODO : Go through current state scene
			SceneGraphRecurse(Main::game.getCurrentState()->getScene().getEntities());
			ImGui::TreePop();
		}

		ImGui::End();
	}

	void SceneGraphRecurse(const std::vector<Entity*>& children)
	{
		for (Entity* child : children)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;

			// Different flags for tree and leaf nodes
			if (child->getChildren().empty())
				flags |= ImGuiTreeNodeFlags_Leaf;
			else
				flags |= ImGuiTreeNodeFlags_OpenOnArrow;

			// Highlight selected node
			if (Main::game.getCurrentState()->getScene().currentActiveEntity == child)
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
			Entity* activeEntity = Main::game.getCurrentState()->getScene().currentActiveEntity;

			if (activeEntity != nullptr)
				activeEntity->drawOutline = false;

			Main::game.getCurrentState()->getScene().currentActiveEntity = object;
			Main::game.getCurrentState()->getScene().currentActiveEntity->drawOutline = true;
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
				if (object == Main::game.getCurrentState()->getScene().currentActiveEntity)
					Main::game.getCurrentState()->getScene().currentActiveEntity = nullptr;

				// TODO : Remove from scene
				if (!Main::game.getCurrentState()->getScene().removeEntity(object))
				{
					object->getParent()->removeChild(object);
					delete object;
				}
			}

			ImGui::EndPopup();
		}
	}

	// TODO : There's definitely a way to make this code much cleaner
	void ShowComponentUI(Component* component)
	{
		if (dynamic_cast<TransformComponent*>(component))
		{
			if (ImGui::CollapsingHeader("Transform"))
			{
				auto* transformComponent = dynamic_cast<TransformComponent*>(component);

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
				auto* skyboxComponent = dynamic_cast<SkyboxComponent*>(component);

				const char* combo_preview_value = skyboxParams.comboSkyboxes[skyboxParams.currentSkyboxId];
				if (ImGui::BeginCombo("##skySelector", combo_preview_value))
				{
					for (int i = 0; i < 3; i++)
					{
						const bool is_selected = (skyboxParams.currentSkyboxId == i);
						if (ImGui::Selectable(skyboxParams.comboSkyboxes[i], is_selected))
						{
							skyboxParams.currentSkyboxId = i;

							switch (skyboxParams.currentSkyboxId)
							{
								case static_cast<int>(SkyboxType::GRASS):
									skyboxComponent->changeSkybox(SkyboxType::GRASS);
									break;

								case static_cast<int>(SkyboxType::NIGHT):
									skyboxComponent->changeSkybox(SkyboxType::NIGHT);
									break;

								case static_cast<int>(SkyboxType::SKY):
									skyboxComponent->changeSkybox(SkyboxType::SKY);
									break;

								default:
									Logger::logError("Got invalid skybox ID when changing skybox!", "interface.cpp");
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
				auto* meshComponent = dynamic_cast<MeshComponent*>(component);

				std::string verticesText = "Vertices: " + std::to_string(meshComponent->getVerticesCount());
				std::string indicesText = "Indices: " + std::to_string(meshComponent->getIndicesCount());

				ImGui::Text("%s", verticesText.c_str());
				ImGui::Text("%s", indicesText.c_str());

				if (ImGui::CollapsingHeader("Bounding box"))
				{
					BoundingBox meshBoundingBox = meshComponent->getWorldBoundingBox();
					glm::vec3 minCorner = meshBoundingBox.minPosition;
					glm::vec3 maxCorner = meshBoundingBox.maxPosition;

					std::string minCornerText = "Min corner: %.2f - %.2f - %.2f";
					std::string maxCornerText = "Max corner: %.2f - %.2f - %.2f";
					
					ImGui::Text(minCornerText.c_str(), minCorner.x, minCorner.y, minCorner.z);
					ImGui::Text(maxCornerText.c_str(), maxCorner.x, maxCorner.y, maxCorner.z);
				}

				auto* phongMaterial = dynamic_cast<PhongMaterial*>(meshComponent->material.get());
				auto* pbrMaterial = dynamic_cast<PBRMaterial*>(meshComponent->material.get());

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->albedoTexture->texID), ImVec2(width, height));

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->normalTexture->texID), ImVec2(width, height));

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->metallicTexture->texID), ImVec2(width, height));

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->roughnessTexture->texID), ImVec2(width, height));

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->aoTexture->texID), ImVec2(width, height));

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->opacityTexture->texID), ImVec2(width, height));

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
						ImGui::Image(reinterpret_cast<ImTextureID>(pbrMaterial->emissiveTexture->texID), ImVec2(width, height));

						if (ImGui::Button("View emissive map"))
						{
							textureViewerParams.showTexture = true;
							textureViewerParams.currentTexture = pbrMaterial->emissiveTexture;
							ImGui::SetWindowFocus("Texture viewer");
						}
					}
				}//
			}
		}
		else if (dynamic_cast<CameraComponent*>(component))
		{
			if (ImGui::CollapsingHeader("Camera"))
			{
				auto* cameraComponent = dynamic_cast<CameraComponent*>(component);

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
				auto* pointLightComponent = dynamic_cast<PointLightComponent*>(component);

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
				auto* spotLightComponent = dynamic_cast<SpotLightComponent*>(component);

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
				auto* directionalLightComponent = dynamic_cast<DirectionalLightComponent*>(component);

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
				auto* scriptComponent = dynamic_cast<ScriptComponent*>(component);

				if (ImGui::Button("Edit script"))
				{
					scriptEditorParams.currentEditedScript = scriptComponent;
					scriptEditorParams.isEditingScript = true;
					scriptEditorParams.editor.SetText(scriptComponent->scriptCode);
					toolbarParams.scriptEditorOn = true;
				}
			}
		}
		else if (dynamic_cast<PhysicsComponent*>(component))
		{
			if (ImGui::CollapsingHeader("Physics"))
			{

			}
		}
	}

	void TextureViewer()
	{
		if (!toolbarParams.textureViewerOn)
			return;

		ImGui::Begin("Texture viewer");

		if (textureViewerParams.showTexture)
		{
			ImVec2 contentRegionSize = ImGui::GetContentRegionAvail();

			// We want the image to always fit in the space of the window
			float width = contentRegionSize.x;
			float height = textureViewerParams.currentTexture->height / textureViewerParams.currentTexture->width * width;
			ImGui::Image(reinterpret_cast<ImTextureID>(textureViewerParams.currentTexture->texID), ImVec2(width, height));

			std::string dimensions = "Size: " + std::to_string(static_cast<int>(textureViewerParams.currentTexture->width)) + "x" + std::to_string(static_cast<int>(textureViewerParams.currentTexture->height)) + " px";
			ImGui::Text("%s", dimensions.c_str());
		}

		ImGui::End();
	}
}
