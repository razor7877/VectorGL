#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "headers/shader.hpp"
#include "headers/interface.hpp"
#include "headers/camera.hpp"

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

void ImGuiDrawWindows(Camera &camera, Shader shaderProgram, float &mixval)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Shader texture mixing value
	ImGui::Begin("Settings");

	ImGui::SliderFloat("mixval", &mixval, 0.0f, 1.0f);
	shaderProgram.setFloat("mixval", mixval);

	ImGui::End();

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

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}