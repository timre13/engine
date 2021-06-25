#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <cassert>
#include "types.h"

class ShaderProgram final
{
public:
    enum class State
    {
        Uninitialized,
        Ok,
        OpenFailed,
        CompileFailed,
        LinkFailed,
    };

private:
    uint m_shaderProgramId{};
    State m_state{State::Uninitialized};

public:
    ShaderProgram(){}

    // Copy ctor, copy assignment op
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    // Move ctor, move assignment op
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    /*
     * Opens a vertex and a fragment shader, compiles and links them.
     *
     * Returns:
     *      true if failed,
     *      false otherwise
     */
    bool open(const std::string& vertexPath, const std::string& fragmentPath);

    inline uint getId() const { return m_shaderProgramId; }
    inline State getState() const { return m_state; }

    inline void use() { assert(m_state == State::Ok); glUseProgram(m_shaderProgramId); }

    ~ShaderProgram();
};

