#include "Texture.h"
#include <SDL2/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG // Get better error messages
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare" // Ignore a warning in the library
#include "../submodules/stb/stb_image.h"
#pragma GCC diagnostic pop

#include "memory.h"
#include "Logger.h"

Texture::Texture(Texture&& another)
{
    m_state = another.m_state;
    m_textureIndex = another.m_textureIndex;
    another.m_textureIndex = 0;
    m_widthPx = another.m_widthPx;
    another.m_widthPx = 0;
    m_heightPx = another.m_heightPx;
    another.m_heightPx = 0;
}

Texture& Texture::operator=(Texture&& another)
{
    if (this == &another)
        return *this;

    m_state = another.m_state;
    m_textureIndex = another.m_textureIndex;
    another.m_textureIndex = 0;
    m_widthPx = another.m_widthPx;
    another.m_widthPx = 0;
    m_heightPx = another.m_heightPx;
    another.m_heightPx = 0;

    return *this;
}

bool Texture::open(const std::string& filePath, int horizontalWrapMode/*=GL_REPEAT*/, int verticalWrapMode/*=GL_REPEAT*/)
{
    unsigned char* textureData;
    int channelCount;
    if (filePath.empty())
    {
        Logger::verb << "Generating empty image" << Logger::End;

        textureData = (unsigned char*)calloc(4, sizeof(unsigned char));
        m_widthPx = 1;
        m_heightPx = 1;
        channelCount = 4;
    }
    else
    {
        Logger::verb << "Reading image: " << filePath << Logger::End;

        stbi_set_flip_vertically_on_load(1);
        textureData = stbi_load(filePath.c_str(), &m_widthPx, &m_heightPx, &channelCount, 4);
        if (!textureData)
        {
            Logger::err << "Failed to open image: " << filePath << ": " << stbi_failure_reason() << Logger::End;
            m_state = State::OpenFailed;
            return 1;
        }
    }

    glGenTextures(1, &m_textureIndex);
    glBindTexture(GL_TEXTURE_2D, m_textureIndex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_widthPx, m_heightPx, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);
    
    free(textureData);

    Logger::verb << "Opened image (size: " << m_widthPx << 'x' << m_heightPx << ')' << Logger::End;
    m_state = State::Ok;
    return 0;
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureIndex);
    Logger::verb << "Deleted a texture (" << this << ')' << Logger::End;
}

