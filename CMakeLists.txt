cmake_minimum_required(VERSION 3.24)
project(VectorGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(OpenGL REQUIRED)

# Add CMake library subfolders
add_subdirectory(libs/assimp EXCLUDE_FROM_ALL)
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)
add_subdirectory(libs/lua-5.4.7 EXCLUDE_FROM_ALL)

set(BULLET_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libs/bullet3")
find_package(Bullet REQUIRED)

# Find all source files
file(GLOB MAIN_SRC "src/*.c" "src/*.cpp")
file(GLOB COMPONENTS_SRC "src/components/*.c" "src/components/*.cpp")
file(GLOB COMPONENTS_LIGHTS_SRC "src/components/lights/*.c" "src/components/lights/*.cpp")
file(GLOB GAME_SRC "src/game/*.c" "src/game/*.cpp")
file(GLOB IO_SRC "src/io/*.c" "src/io/*.cpp")
file(GLOB KHR_SRC "src/KHR/*.c" "src/KHR/*.cpp")
file(GLOB MATERIALS_SRC "src/materials/*.c" "src/materials/*.cpp")
file(GLOB PHYSICS_SRC "src/physics/*.c" "src/physics/*.cpp")
file(GLOB UTILITIES_SRC "src/utilities/*.c" "src/utilities/*.cpp")

# Find all header files
file(GLOB MAIN_INCLUDES "includes/*.h" "includes/*.hpp")
file(GLOB COMPONENTS_INCLUDES "includes/components/*.h" "includes/components/*.hpp")
file(GLOB COMPONENTS_LIGHTS_INCLUDES "includes/components/lights/*.h" "includes/components/lights/*.hpp")
file(GLOB GAME_INCLUDES "includes/game/*.h" "includes/game/*.hpp")
file(GLOB IO_INCLUDES "includes/io/*.h" "includes/io/*.hpp")
file(GLOB KHR_INCLUDES "includes/KHR/*.h" "includes/KHR/*.hpp")
file(GLOB MATERIALS_INCLUDES "includes/materials/*.h" "includes/materials/*.hpp")
file(GLOB PHYSICS_INCLUDES "includes/physics/*.h" "includes/physics/*.hpp")
file(GLOB UTILITIES_INCLUDES "includes/utilities/*.h" "includes/utilities/*.hpp")

# Add sources to target
add_executable(VectorGL
	${MAIN_SRC}
	${COMPONENTS_SRC}
	${COMPONENTS_LIGHTS_SRC}
	${GAME_SRC}
	${IO_SRC}
	${KHR_SRC}
	${MATERIALS_SRC}
	${PHYSICS_SRC}
	${UTILITIES_SRC}

	${MAIN_INCLUDES}
	${COMPONENTS_INCLUDES}
	${COMPONENTS_LIGHTS_INCLUDES}
	${GAME_INCLUDES}
	${IO_INCLUDES}
	${KHR_INCLUDES}
	${MATERIALS_INCLUDES}
	${PHYSICS_INCLUDES}
	${UTILITIES_INCLUDES}

	${LUA_SRC_INCLUDES}

	libs/ImGuiColorTextEdit/TextEditor.cpp
	libs/ImGuiColorTextEdit/TextEditor.h
	
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
source_group("src\\game" FILES ${GAME_SRC})
source_group("src\\io" FILES ${IO_SRC})
source_group("src\\KHR" FILES ${KHR_SRC})
source_group("src\\materials" FILES ${MATERIALS_SRC})
source_group("src\\physics" FILES ${PHYSICS_SRC})
source_group("src\\utilities" FILES ${UTILITIES_SRC})

source_group("includes" FILES ${MAIN_INCLUDES})
source_group("includes\\components" FILES ${COMPONENTS_INCLUDES})
source_group("includes\\components\\lights" FILES ${COMPONENTS_LIGHTS_INCLUDES})
source_group("includes\\game" FILES ${GAME_INCLUDES})
source_group("includes\\io" FILES ${IO_INCLUDES})
source_group("includes\\KHR" FILES ${KHR_INCLUDES})
source_group("includes\\materials" FILES ${MATERIALS_INCLUDES})
source_group("includes\\physics" FILES ${PHYSICS_INCLUDES})
source_group("includes\\utilities" FILES ${UTILITIES_INCLUDES})

# Add header files
target_include_directories(VectorGL PUBLIC
	includes
	libs
	libs/glm
	libs/ImGui
	libs/lua-5.4.7/src
	${BULLET_INCLUDE_DIR}
)

MESSAGE(${BULLET_INCLUDE_DIR})

target_link_libraries(VectorGL PUBLIC
	assimp
	glfw
	lua
	${BULLET_LIBRARIES}
	OpenGL::GL)

add_compile_definitions(IMGUI_USER_CONFIG="io/imguiConfig.hpp")

# Copy all assets to build folder
file(COPY src/shaders DESTINATION ${VectorGL_BINARY_DIR})
file(COPY img DESTINATION ${VectorGL_BINARY_DIR})
file(COPY models DESTINATION ${VectorGL_BINARY_DIR})
# Copy ImGui config to not need to move windows around every time
file(COPY imgui.ini DESTINATION ${VectorGL_BINARY_DIR})