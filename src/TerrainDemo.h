//
// Created by Lucas Wang on 2024-06-14.
//

#ifndef TERRAINRENDERING_TERRAINDEMO_H
#define TERRAINRENDERING_TERRAINDEMO_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "shader.h"
#include "terrain.h"
#include <iostream>


class TerrainDemo {
public:
    TerrainDemo();
    ~TerrainDemo();

    void Init();
    void Run();

    void OnResize(GLFWwindow*, int, int);
    void OnMouseMove(GLFWwindow*, double, double);
    void OnKeyPressed(GLFWwindow*, int, int, int, int);

    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;

    bool firstMouse = true;
    float m_lastX;
    float m_lastY;


    int m_keys[1024];
private:
    GLFWwindow *m_window;
    Camera *m_camera;
    Shader *m_shader;
    Terrain *m_terrain;

    void CreateWindow();
    void CreateShaders();
    void CreateCamera();
    void ProcessInput();

    void SetCallbacks();
    void SetShaderUniforms();

    void InitTerrain();
    void Render();

    void InitImGui();

};


#endif //TERRAINRENDERING_TERRAINDEMO_H
