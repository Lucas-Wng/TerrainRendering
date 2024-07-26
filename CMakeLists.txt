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
        include/stb_image.h
        src/stb_image.cpp
        src/HeightMap.cpp
        src/HeightMap.h
        src/PerlinNoise.cpp
        src/PerlinNoise.h
        src/texture.h
)

target_include_directories(TerrainRendering PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(TerrainRendering PRIVATE OpenGL::GL glfw)