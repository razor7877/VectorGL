cmake_minimum_required(VERSION 3.28)
project(VectorGL)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# Add library subfolders
add_subdirectory(libs/assimp EXCLUDE_FROM_ALL)
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)

# Recursively find all source files
file(GLOB_RECURSE sources src/*(.c|cpp) includes/*(.h/.hpp))

# Add sources to target
add_executable(VectorGL ${sources})
target_sources(VectorGL PUBLIC
	libs/ImGui/imgui.cpp
	libs/ImGui/imgui_demo.cpp
	libs/ImGui/imgui_draw.cpp
	libs/ImGui/backends/imgui_impl_glfw.cpp
	libs/ImGui/backends/imgui_impl_opengl3.cpp
	libs/ImGui/imgui_tables.cpp
	libs/ImGui/imgui_widgets.cpp
)

# Add header files
target_include_directories(VectorGL PUBLIC
	includes
	src/headers
	libs
	libs/glm
	libs/ImGui
)

target_link_libraries(VectorGL PUBLIC
	assimp
	glfw
	OpenGL::GL)

# Copy all assets to build folder
file(COPY src/shaders DESTINATION ${VectorGL_BINARY_DIR})
file(COPY img DESTINATION ${VectorGL_BINARY_DIR})
file(COPY models DESTINATION ${VectorGL_BINARY_DIR})
# Copy ImGui config to not need to move windows around every time
file(COPY imgui.ini DESTINATION ${VectorGL_BINARY_DIR})