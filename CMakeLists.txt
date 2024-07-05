cmake_minimum_required(VERSION 3.24)
project(VectorGL)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

# Add library subfolders
add_subdirectory(libs/assimp EXCLUDE_FROM_ALL)
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)

# Find all source files
file(GLOB MAIN_SRC "src/*.c" "src/*.cpp")
file(GLOB COMPONENTS_SRC "src/components/*.c" "src/components/*.cpp")
file(GLOB COMPONENTS_LIGHTS_SRC "src/components/lights/*.c" "src/components/lights/*.cpp")
file(GLOB IO_SRC "src/io/*.c" "src/io/*.cpp")
file(GLOB KHR_SRC "src/KHR/*.c" "src/KHR/*.cpp")
file(GLOB LIGHTS_SRC "src/lights/*.c" "src/lights/*.cpp")
file(GLOB UTILITIES_SRC "src/utilities/*.c" "src/utilities/*.cpp")

# Find all header files
file(GLOB MAIN_INCLUDES "includes/*.h" "includes/*.hpp")
file(GLOB COMPONENTS_INCLUDES "includes/components/*.h" "includes/components/*.hpp")
file(GLOB COMPONENTS_LIGHTS_INCLUDES "includes/components/lights/*.h" "includes/components/lights/*.hpp")
file(GLOB IO_INCLUDES "includes/io/*.h" "includes/io/*.hpp")
file(GLOB KHR_INCLUDES "includes/KHR/*.h" "includes/KHR/*.hpp")
file(GLOB LIGHTS_INCLUDES "includes/lights/*.h" "includes/lights/*.hpp")
file(GLOB UTILITIES_INCLUDES "includes/utilities/*.h" "includes/utilities/*.hpp")

message(STATUS "C files: ${COMPONENTS_INCLUDES}")

# Add sources to target
add_executable(VectorGL
	${MAIN_SRC}
	${COMPONENTS_SRC}
	${COMPONENTS_LIGHTS_SRC}
	${IO_SRC}
	${KHR_SRC}
	${LIGHTS_SRC}
	${UTILITIES_SRC}

	${MAIN_INCLUDES}
	${COMPONENTS_INCLUDES}
	${COMPONENTS_LIGHTS_INCLUDES}
	${IO_INCLUDES}
	${KHR_INCLUDES}
	${LIGHTS_INCLUDES}
	${UTILITIES_INCLUDES}

	libs/ImGui/imgui.cpp
	libs/ImGui/imgui_demo.cpp
	libs/ImGui/imgui_draw.cpp
	libs/ImGui/backends/imgui_impl_glfw.cpp
	libs/ImGui/backends/imgui_impl_opengl3.cpp
	libs/ImGui/imgui_tables.cpp
	libs/ImGui/imgui_widgets.cpp
	libs/ImGui/misc/cpp/imgui_stdlib.cpp
)

source_group("src" FILES ${MAIN_SRC})
source_group("src\\components" FILES ${COMPONENTS_SRC})
source_group("src\\components\\lights" FILES ${COMPONENTS_LIGHTS_SRC})
source_group("src\\io" FILES ${IO_SRC})
source_group("src\\KHR" FILES ${KHR_SRC})
source_group("src\\lights" FILES ${LIGHTS_SRC})
source_group("src\\utilities" FILES ${UTILITIES_SRC})

source_group("includes" FILES ${MAIN_INCLUDES})
source_group("includes\\components" FILES ${COMPONENTS_INCLUDES})
source_group("includes\\components\\lights" FILES ${COMPONENTS_LIGHTS_INCLUDES})
source_group("includes\\io" FILES ${IO_INCLUDES})
source_group("includes\\KHR" FILES ${KHR_INCLUDES})
source_group("includes\\lights" FILES ${LIGHTS_INCLUDES})
source_group("includes\\utilities" FILES ${UTILITIES_INCLUDES})

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