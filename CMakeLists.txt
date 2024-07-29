cmake_minimum_required(VERSION 3.28)
project(TerrainRendering)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(TerrainRendering src/main.cpp
        src/glad.c
        src/camera.h
        src/terrain.cpp
        src/shader.h
        src/TerrainDemo.cpp
        src/TerrainDemo.h
        src/stb_image.cpp
        src/HeightMap.cpp
        src/HeightMap.h
        src/PerlinNoise.cpp
        src/PerlinNoise.h
        src/TextureLoader.cpp
        src/TextureLoader.h
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        src/Skybox.cpp
        src/Skybox.h
)

target_include_directories(TerrainRendering PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(TerrainRendering PRIVATE ${CMAKE_SOURCE_DIR}/include/imgui)
target_include_directories(TerrainRendering PRIVATE ${CMAKE_SOURCE_DIR}/include/imgui/backends)

target_link_libraries(TerrainRendering PRIVATE OpenGL::GL glfw)