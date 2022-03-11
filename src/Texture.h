#pragma once

#include "types.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

class Texture final
{
public:
    enum class State
    {
        Uninitialized,
        Ok,
        OpenFailed,
    };

private:
    State m_state{State::Uninitialized};
    uint m_textureIndex{};
    int m_widthPx{};
    int m_heightPx{};

public:
    Texture(const std::string& filePath, int horizontalWrapMode=GL_REPEAT, int verticalWrapMode=GL_REPEAT);
    // Copy ctor, copy assignment op
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Move ctor, move assignment op
    Texture(Texture&& another);
    Texture& operator=(Texture&& another);

    inline State getState() const { return m_state; }
    inline int getWidth() const { return m_widthPx; }
    inline int getHeight() const { return m_heightPx; }

    inline void setWrapMode(int horizontalWrapMode, int verticalWrapMode)
    {
        glBindTexture(GL_TEXTURE_2D, m_textureIndex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);
    }

    inline void bind() { glBindTexture(GL_TEXTURE_2D, m_textureIndex); }

    ~Texture();
};

