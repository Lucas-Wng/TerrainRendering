//
// Created by Lucas Wang on 2024-06-12.
//

#include "terrain.h"
#include <iostream>

Terrain::Terrain() {

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

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}



void Terrain::PopulateBuffer() {
    std::vector<Vertex> Vertices;
    Vertices.resize(m_width * m_depth);

    InitVertices(Vertices);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::InitVertices(std::vector<Vertex>& Vertices) {
    int index = 0;
    for (int z = 0; z < m_depth; z++) {
        for (int x = 0; x < m_width; x++) {
            float height = m_HeightMap->getMData()[index];
            Vertices[index].InitVertex(x/50.0f, height * 2.0f, z/50.0f);
            index++;
        }
    }
}

void Terrain::Vertex::InitVertex(float x, float y, float z) {
    Pos = glm::vec3(x, y, z);
}

void Terrain::Render() {
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_width * m_depth);
    glBindVertexArray(0);
}

void Terrain::InitHeightMap() {
    m_HeightMap = new HeightMap();
    m_HeightMap->LoadFileHeightMap("resources/heightmaps/iceland_heightmap.png");
    m_width = m_HeightMap->getMWidth();
    m_depth = m_HeightMap->getMHeight();
}
