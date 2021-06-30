#pragma once

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
    unsigned int m_textureIndex{};

public:
    Texture(){}
    // Copy ctor, copy assignment op
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Move ctor, move assignment op
    Texture(Texture&& another);
    Texture& operator=(Texture&& another);

    bool open(const std::string& filePath, int horizontalWrapMode=GL_REPEAT, int verticalWrapMode=GL_REPEAT);

    inline State getState() const { return m_state; }

    inline void bind() { glBindTexture(GL_TEXTURE_2D, m_textureIndex); }

    ~Texture();
};

