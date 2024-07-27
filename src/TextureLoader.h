//
// Created by Lucas Wang on 2024-07-26.
//

#ifndef TERRAINRENDERING_TEXTURELOADER_H
#define TERRAINRENDERING_TEXTURELOADER_H


#include <glad/glad.h>
#include <string>

class TextureLoader {
public:
    static GLuint LoadTexture(const std::string& filePath, GLenum textureUnit);
};


#endif //TERRAINRENDERING_TEXTURELOADER_H
