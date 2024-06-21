//
// Created by Lucas Wang on 2024-06-12.
//

#ifndef TERRAINRENDERING_TERRAIN_H
#define TERRAINRENDERING_TERRAIN_H

#include "HeightMap.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class Terrain {
public:
    Terrain();
    ~Terrain();

    void Render();
    void Generate();
private:
    struct Vertex {
        glm::vec3 Pos;

        void InitVertex(float x, float y, float z);
    };

    int m_width;
    int m_depth;
    HeightMap* m_HeightMap;
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;


    void PopulateBuffer();
    void InitVertices(std::vector<Vertex>& Vertices);
    void InitGLStates();
    void InitHeightMap();
};


#endif //TERRAINRENDERING_TERRAIN_H
