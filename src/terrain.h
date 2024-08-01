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
//#include "PerlinNoise.h"
#include <unordered_map>
#include <utility>
#include "PerlinNoise.hpp"


class Terrain {
public:
    Terrain(int width, int depth, bool perlinNoise = false);
    Terrain(int x, int z, int chunkSize, float terrainScale);

    void Render();
    void Generate();
private:
    struct Vertex {
        glm::vec3 Pos;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec2 TexCoords;
        void InitVertex(double x, double y, double z, double u, double v);
    };

    int chunkX;
    int chunkZ;

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise m_perlin{ seed };

    bool m_perlinNoise = false;
    int m_width;
    int m_depth;
    float m_terrainScale;
    std::vector<double> m_heightmap;
    std::vector<glm::vec3> m_normals;
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    unsigned int m_EBO = 0;

    void PopulateBuffer();
    void InitVertices(std::vector<Vertex>& Vertices);
    void InitGLStates();
    void InitHeightMap();
    void InitIndices(std::vector<unsigned int>& Indices);
    void ComputeNormalsAndTangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void UnbindBuffers();
    void SetupVertexAttribs(GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
    template <typename T>
    void UploadBufferData(GLuint buffer, const T* data, GLsizeiptr size);
};


#endif //TERRAINRENDERING_TERRAIN_H
