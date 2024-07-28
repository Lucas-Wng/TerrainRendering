//
// Created by Lucas Wang on 2024-06-12.
//

#include "terrain.h"
#include <iostream>

Terrain::Terrain(int width, int depth, bool perlinNoise)
        : m_width(width), m_depth(depth), m_perlinNoise(perlinNoise) {

    LoadTextures();
}

void Terrain::LoadTextures() {
//    m_diffuseTexture = TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_diff_4k.png",GL_TEXTURE0);
//    m_displacementTexture = TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_disp_4k.png", GL_TEXTURE1);
//    m_normalTexture = TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_nor_gl_4k.png", GL_TEXTURE2);
//    m_roughTexture = TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_rough_4k.png", GL_TEXTURE3);
    m_diffuseTexture = TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_diff_4k.jpg", GL_TEXTURE0);
    m_displacementTexture = TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_disp_4k.png", GL_TEXTURE1);
    m_normalTexture = TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_nor_gl_4k.png", GL_TEXTURE2);
    m_roughTexture = TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_rough_4k.png", GL_TEXTURE3);
}

void Terrain::Generate() {
    InitGLStates();
    InitHeightMap();
    PopulateBuffer();
    UnbindBuffers();
}

void Terrain::InitGLStates() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    SetupVertexAttribs(m_VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Pos));
    SetupVertexAttribs(m_VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    SetupVertexAttribs(m_VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    SetupVertexAttribs(m_VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    glBindVertexArray(0);
}

void Terrain::SetupVertexAttribs(GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, pointer);
}

void Terrain::PopulateBuffer() {
    std::vector<Vertex> vertices(m_width * m_depth);
    InitVertices(vertices);

    std::vector<unsigned int> indices;
    InitIndices(indices);
    ComputeNormals(vertices, indices);
    ComputeTangents(vertices, indices);

    UploadBufferData(m_VBO, vertices.data(), sizeof(Vertex) * vertices.size());
    UploadBufferData(m_EBO, indices.data(), sizeof(unsigned int) * indices.size());
}

template <typename T>
void Terrain::UploadBufferData(GLuint buffer, const T* data, GLsizeiptr size) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Terrain::InitVertices(std::vector<Vertex>& vertices) {
    float scale = 100.0f;
    for (int z = 0, index = 0; z < m_depth; ++z) {
        for (int x = 0; x < m_width; ++x, ++index) {
            double height = m_heightmap[index];
            float u = static_cast<float>(x) / m_width * scale;
            float v = static_cast<float>(z) / m_depth * scale;
            vertices[index].InitVertex(x / 50.0, height * 6.0, z / 50.0, u, v);
        }
    }
}

void Terrain::Vertex::InitVertex(double x, double y, double z, double u, double v) {
    Pos = glm::vec3(x, y, z);
    Normal = glm::vec3(0.0f);
    Tangent = glm::vec3(0.0f);
    TexCoords = glm::vec2(u, v);
}

void Terrain::Render() {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, (m_width - 1) * (m_depth - 1) * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Terrain::InitHeightMap() {
    if (m_perlinNoise) {
        PerlinNoise perlinNoise(m_width, m_depth);
        perlinNoise.GenerateHeightMap();
        m_heightmap = perlinNoise.GetHeightMap();
    } else {
        HeightMap heightMap;
        heightMap.LoadFileHeightMap("resources/heightmaps/iceland_heightmap.png");
        m_width = heightMap.getMWidth();
        m_depth = heightMap.getMHeight();
        m_heightmap = heightMap.getMData();
    }
}

void Terrain::InitIndices(std::vector<unsigned int>& indices) {
    for (int z = 0; z < m_depth - 1; ++z) {
        for (int x = 0; x < m_width - 1; ++x) {
            int topLeft = (z * m_width) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((z + 1) * m_width) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void Terrain::ComputeNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Initialize normals to zero
    std::vector<glm::vec3> vertexNormals(vertices.size(), glm::vec3(0.0f));

    // Iterate over each triangle
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        Vertex& v0 = vertices[i0];
        Vertex& v1 = vertices[i1];
        Vertex& v2 = vertices[i2];

        glm::vec3 edge1 = v1.Pos - v0.Pos;
        glm::vec3 edge2 = v2.Pos - v0.Pos;

        // Compute face normal
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        // Accumulate the face normal to the vertices involved in the face
        vertexNormals[i0] += faceNormal;
        vertexNormals[i1] += faceNormal;
        vertexNormals[i2] += faceNormal;
    }

    // Normalize the accumulated vertex normals
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].Normal = glm::normalize(vertexNormals[i]);
    }
}

void Terrain::ComputeTangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.Pos - v0.Pos;
        glm::vec3 edge2 = v2.Pos - v0.Pos;

        glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
        glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        v0.Tangent += tangent;
        v1.Tangent += tangent;
        v2.Tangent += tangent;
    }

    // Normalize the tangents
    for (auto& vertex : vertices) {
        vertex.Tangent = glm::normalize(vertex.Tangent);
    }
}

void Terrain::UnbindBuffers() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}