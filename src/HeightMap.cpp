//
// Created by Lucas Wang on 2024-06-21.
//

#include "HeightMap.h"

HeightMap::HeightMap() {

}

void HeightMap::LoadFileHeightMap(const std::string &filename) {
    int channels;
    unsigned char* img = stbi_load(filename.c_str(), &m_width, &m_height, &channels, 1);
    if (!img) {
        throw std::runtime_error("Failed to load image");
    }

    m_data.resize(m_width * m_height);

    // Convert the pixel values to floats in the range [0, 1]
    for (int i = 0; i < m_width * m_height; ++i) {
        m_data[i] = img[i] / 255.0;
    }

    stbi_image_free(img);
}
int HeightMap::getMHeight() const {
    return m_height;
}
void HeightMap::setMHeight(int mHeight) {
    m_height = mHeight;
}
int HeightMap::getMWidth() const {
    return m_width;
}
void HeightMap::setMWidth(int mWidth) {
    m_width = mWidth;
}
const std::vector<double> &HeightMap::getMData() const {
    return m_data;
}
void HeightMap::setMData(const std::vector<double> &mData) {
    m_data = mData;
}
