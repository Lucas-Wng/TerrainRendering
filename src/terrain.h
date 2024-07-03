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
#include "PerlinNoise.h"


class Terrain {
public:
    Terrain(int width, int depth, bool perlinNoise = false);
    ~Terrain();

    void Render();
    void Generate();
private:
    struct Vertex {
        glm::vec3 Pos;

        void InitVertex(double x, double y, double z);
    };

    bool m_perlinNoise = false;
    int m_width;
    int m_depth;
    std::vector<double> m_heightmap;
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;


    void PopulateBuffer();
    void InitVertices(std::vector<Vertex>& Vertices);
    void InitGLStates();
    void InitHeightMap();
    void InitIndices(std::vector<unsigned int>& Indices);
};


#endif //TERRAINRENDERING_TERRAIN_H
