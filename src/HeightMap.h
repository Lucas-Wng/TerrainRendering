//
// Created by Lucas Wang on 2024-06-21.
//

#ifndef TERRAINRENDERING_HEIGHTMAP_H
#define TERRAINRENDERING_HEIGHTMAP_H

#include "stb_image.h"
#include <iostream>
#include <vector>

class HeightMap {
public:
    HeightMap();
    void LoadFileHeightMap(const std::string& filename);
private:
    int m_width;
    int m_height;

public:
    int getMHeight() const;
    void setMHeight(int mHeight);
    int getMWidth() const;
    void setMWidth(int mWidth);
    const std::vector<float> &getMData() const;
    void setMData(const std::vector<float> &mData);

private:
    std::vector<float> m_data;
};


#endif//TERRAINRENDERING_HEIGHTMAP_H
