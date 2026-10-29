cmake_minimum_required(VERSION 3.26)
project(solar_sys LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release when no build type is given (single-config generators only)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- External dependencies ---------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

add_library(glad STATIC include/glad.c)
target_include_directories(glad SYSTEM PUBLIC include) # glad/, KHR/, stb_image.h

add_library(imgui STATIC
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui SYSTEM PUBLIC
    include/imgui 
    include/imgui/backends
)
target_link_directories(imgui PUBLIC glfw)

add_executable(solar_sys
    src/main.cpp
    src/Camera.cpp
    src/Material.cpp
    src/Mesh.cpp
    src/MeshData.cpp
    src/Planet.cpp
    src/Player.cpp
    src/ProceduralPlanet.cpp
    src/Shader.cpp
    src/SkyBox.cpp
    src/World.cpp
)
target_link_libraries(solar_sys PRIVATE glad imgui glfw glm::glm OpenGL::GL)

if(MSVC)
    target_compile_options(solar_sys PRIVATE /W4)
else()
    target_compile_options(solar_sys PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- Shaders: copy next to the executable on every build ---------------------
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
            ${CMAKE_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:solar_sys>/shaders
    COMMENT "Copying shaders"
)
add_dependencies(solar_sys copy_shaders)