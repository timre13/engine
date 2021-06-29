#include "Texture.h"
#include <SDL2/SDL_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG // Get better error messages
#include "../submodules/stb/stb_image.h"
#include "Logger.h"

bool Texture::open(const std::string& filePath, int horizontalWrapMode/*=GL_REPEAT*/, int verticalWrapMode/*=GL_REPEAT*/)
{
    stbi_set_flip_vertically_on_load(1);
    int width, height, channelCount;
    unsigned char* textureData = stbi_load(filePath.c_str(), &width, &height, &channelCount, 4);
    if (!textureData)
    {
        Logger::err << "Failed to open image: " << filePath << ": " << stbi_failure_reason() << Logger::End;
        m_state = State::OpenFailed;
        return 1;
    }

    glGenTextures(1, &m_textureIndex);
    glBindTexture(GL_TEXTURE_2D, m_textureIndex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(textureData);

    Logger::verb << "Opened image: " << filePath << Logger::End;
    m_state = State::Ok;
    return 0;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureIndex);
    Logger::verb << "Deleted a texture" << Logger::End;
}

