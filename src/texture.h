//
// Created by Lucas Wang on 2024-07-10.
//

#ifndef TERRAINRENDERING_TEXTURE_H
#define TERRAINRENDERING_TEXTURE_H
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <string>

class Texture {
public:
    GLuint ID;
    std::string type;
    std::string path;

    Texture(const std::string& texturePath, const std::string& textureType) : path(texturePath), type(textureType) {
        std::cout << "Loading texture: " << texturePath << std::endl;
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // Set texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR); // Repeat texture horizontally
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR); // Repeat texture vertically

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load image
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // Flip the texture if needed
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            std::cout << "Successfully loaded texture: " << texturePath << std::endl;
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            else {
                std::cerr << "Unsupported number of channels: " << nrChannels << std::endl;
                stbi_image_free(data);
                return;
            }

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
            std::cerr << "STB Image error: " << stbi_failure_reason() << std::endl;
        }
        stbi_image_free(data);
    }

    void Bind() const {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};

#endif //TERRAINRENDERING_TEXTURE_H