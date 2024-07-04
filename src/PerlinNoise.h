//
// Created by Lucas Wang on 2024-07-03.
//

#ifndef TERRAINRENDERING_PERLINNOISE_H
#define TERRAINRENDERING_PERLINNOISE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

class PerlinNoise {
public:

    PerlinNoise(int width, int depth);

    void GenerateHeightMap();
    std::vector<double> GetHeightMap();

private:
    std::vector<double> m_data;
    int m_width;
    int m_depth;
};


#endif//TERRAINRENDERING_PERLINNOISE_H
