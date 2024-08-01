//
// Created by Lucas Wang on 2024-08-01.
//

#include "InfiniteTerrain.h"
#include <iostream>
#include "TextureLoader.h"

InfiniteTerrain::InfiniteTerrain(int chunkSize, float terrainScale) : m_chunkSize(chunkSize), m_terrainScale(terrainScale) {
    LoadTextures();
}

void InfiniteTerrain::LoadTextures() {
//    TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_diff_4k.png",GL_TEXTURE0);
//    TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_disp_4k.png", GL_TEXTURE1);
//    TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_nor_gl_4k.png", GL_TEXTURE2);
//    TextureLoader::LoadTexture("resources/textures/metal_grate_rusty/metal_grate_rusty_rough_4k.png", GL_TEXTURE3);
    TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_diff_4k.jpg", GL_TEXTURE0);
    TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_disp_4k.png", GL_TEXTURE1);
    TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_nor_gl_4k.png", GL_TEXTURE2);
    TextureLoader::LoadTexture("resources/textures/rock_face/rock_face_03_rough_4k.png", GL_TEXTURE3);
//    TextureLoader::LoadTexture("resources/textures/coast_sand_rocks/coast_sand_rocks_02_diff_4k.png",GL_TEXTURE0);
//    TextureLoader::LoadTexture("resources/textures/coast_sand_rocks/coast_sand_rocks_02_disp_4k.png", GL_TEXTURE1);
//    TextureLoader::LoadTexture("resources/textures/coast_sand_rocks/coast_sand_rocks_02_nor_gl_4k.png", GL_TEXTURE2);
//    TextureLoader::LoadTexture("resources/textures/coast_sand_rocks/coast_sand_rocks_02_rough_4k.png", GL_TEXTURE3);
}

void InfiniteTerrain::updateChunks(float cameraX, float cameraZ) {
    int currentChunkX = static_cast<int>(cameraX * m_terrainScale / m_chunkSize);
    int currentChunkZ = static_cast<int>(cameraZ * m_terrainScale / m_chunkSize);
//    std::cout << "Current chunk: " << currentChunkX << " " << currentChunkZ << std::endl;
    for (int dz = -2; dz <= 2; dz++) {
        for (int dx = -2; dx <= 2; dx++) {
            int newX = currentChunkX + dx;
            int newZ = currentChunkZ + dz;
            auto chunkPos = std::make_pair(newX, newZ);

            if (chunks.find(chunkPos) == chunks.end()) {
                chunks[chunkPos] = new Terrain(newX, newZ, m_chunkSize, m_terrainScale);
                chunks[chunkPos]->Generate();
            }
        }
    }
}

void InfiniteTerrain::renderTerrain() {
    for (auto& pair : chunks) {
        pair.second->Render();
    }
}

void InfiniteTerrain::cleanupChunks(float cameraX, float cameraZ) {
    int currentChunkX = static_cast<int>(cameraX * m_terrainScale / m_chunkSize);
    int currentChunkZ = static_cast<int>(cameraZ * m_terrainScale / m_chunkSize);

    for (auto it = chunks.begin(); it != chunks.end();) {
        int chunkX = it->first.first;
        int chunkZ = it->first.second;

        if (std::abs(chunkX - currentChunkX) > 2 || std::abs(chunkZ - currentChunkZ) > 2) {
            delete it->second;
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}