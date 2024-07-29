//
// Created by Lucas Wang on 2024-07-29.
//

#ifndef TERRAINRENDERING_SKYBOX_H
#define TERRAINRENDERING_SKYBOX_H

#include <vector>
#include <string>

class Skybox {
public:
    Skybox();
    unsigned int LoadCubeMap();
    void RenderSkybox();
private:
    std::vector<std::string> m_faces;

    unsigned int m_cubemapTexture;
    unsigned int m_skyboxVAO;
    unsigned int m_skyboxVBO;


    void InitGLStates();
    float m_skyboxVertices[108];
};


#endif //TERRAINRENDERING_SKYBOX_H
