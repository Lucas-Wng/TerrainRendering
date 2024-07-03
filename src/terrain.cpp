//
// Created by Lucas Wang on 2024-06-12.
//

#include "terrain.h"
#include <iostream>

Terrain::Terrain(int width, int depth, bool perlinNoise) {
    m_width = width;
    m_depth = depth;
    m_perlinNoise = perlinNoise;
}

void Terrain::Generate() {
    InitGLStates();
    InitHeightMap();
    PopulateBuffer();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::InitGLStates() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}



void Terrain::PopulateBuffer() {
    std::vector<Vertex> Vertices;
    Vertices.resize(m_width * m_depth);
    InitVertices(Vertices);

    std::vector<unsigned int> Indices;
    InitIndices(Indices);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), Indices.data(), GL_STATIC_DRAW);}

void Terrain::InitVertices(std::vector<Vertex>& Vertices) {
    int index = 0;
    for (int z = 0; z < m_depth; z++) {
        for (int x = 0; x < m_width; x++) {
            double height = m_heightmap[index];
            Vertices[index].InitVertex(x/50.0, height * 2.0, z/50.0);
            index++;
        }
    }
}

void Terrain::Vertex::InitVertex(double x, double y, double z) {
    Pos = glm::vec3(x, y, z);
}

void Terrain::Render() {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, (m_width - 1) * (m_depth - 1) * 6, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, m_width * m_depth);
    glBindVertexArray(0);
}

void Terrain::InitHeightMap() {
    if (m_perlinNoise) {
        PerlinNoise PerlinNoise(m_width, m_depth);
        PerlinNoise.GenerateHeightMap();
        m_heightmap = PerlinNoise.GetHeightMap();
    }
    else {
        HeightMap HeightMap;
        HeightMap.LoadFileHeightMap("resources/heightmaps/iceland_heightmap.png");
        m_width = HeightMap.getMWidth();
        m_depth = HeightMap.getMHeight();
        m_heightmap = HeightMap.getMData();
    }

}
void Terrain::InitIndices(std::vector<unsigned int>& Indices) {
    for (int z = 0; z < m_depth - 1; z++) {
        for (int x = 0; x < m_width - 1; x++) {
            int topLeft = (z * m_width) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((z + 1) * m_width) + x;
            int bottomRight = bottomLeft + 1;

            Indices.push_back(topLeft);
            Indices.push_back(bottomLeft);
            Indices.push_back(topRight);

            Indices.push_back(topRight);
            Indices.push_back(bottomLeft);
            Indices.push_back(bottomRight);
        }
    }
}
