//
// Created by Lucas Wang on 2024-08-01.
//

#ifndef TERRAINRENDERING_INFINITETERRAIN_H
#define TERRAINRENDERING_INFINITETERRAIN_H

#include <unordered_map>
#include <utility>
#include "terrain.h"

class InfiniteTerrain {
public:
    InfiniteTerrain(int chunkSize, float terrainScale);
    void updateChunks(float cameraX, float cameraZ);
    void renderTerrain();
    void cleanupChunks(float cameraX, float cameraZ);
    void LoadTextures();
private:
    int m_chunkSize;
    float m_terrainScale;
    struct hash_pair {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2>& p) const {
            auto hash1 = std::hash<T1>{}(p.first);
            auto hash2 = std::hash<T2>{}(p.second);
            return hash1 ^ hash2;
        }
    };
    std::unordered_map<std::pair<int, int>, Terrain*, hash_pair> chunks;
};


#endif //TERRAINRENDERING_INFINITETERRAIN_H
