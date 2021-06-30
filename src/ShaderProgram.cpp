#include "ShaderProgram.h"
#include "Logger.h"
#include <SDL2/SDL_opengl_glext.h>
#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>

/*
 * Reads the file and sets output to its contents.
 *
 * Returns:
 *      true on error,
 *      false otherwise
 */
static bool getFileContents(const std::string& filePath, std::string* output)
{
    Logger::verb << "Reading shader file: " << filePath << Logger::End;

    std::ifstream fileObject;
    try
    {
        fileObject.open(filePath);
        if (!fileObject.is_open())
        {
            Logger::err << "Failed to open file: " << filePath << ": " << strerror(errno) << Logger::End;
            return 1;
        }
        std::stringstream sstream;
        sstream << fileObject.rdbuf();
        *output = sstream.str();
    }
    catch (std::exception& e)
    {
        Logger::err << "Failed to read shader: " << filePath << ": " << e.what() << Logger::End;
        fileObject.close();
        return 1;
    }

    Logger::verb << "Shader code:\n" << *output << Logger::End;

    fileObject.close();
    return 0;
}

static bool compileShader(uint shaderId)
{
    Logger::verb << "Compiling shader" << Logger::End;

    glCompileShader(shaderId);
    int compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus)
    {
        char errMsg[1024]{};
        glGetShaderInfoLog(shaderId, 1024, nullptr, errMsg);
        Logger::err << "Failed to compile shader: " << errMsg << Logger::End;
        return 1;
    }
    return 0;
}

bool ShaderProgram::open(const std::string& vertexPath, const std::string& fragmentPath)
{
    uint vertexId, fragmentId;

    // Vertex shader
    {
        vertexId = glCreateShader(GL_VERTEX_SHADER);

        std::string vertexSource;
        if (getFileContents(vertexPath, &vertexSource))
        {
            m_state = State::OpenFailed;
            return 1;
        }
        const char* vertexSourceCStr = vertexSource.c_str();
        glShaderSource(vertexId, 1, &vertexSourceCStr, nullptr);

        if (compileShader(vertexId))
        {
            m_state = State::CompileFailed;
            return 1;
        }
    }

    // Fragment shader
    {
        fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

        std::string fragmentSource;
        if (getFileContents(fragmentPath, &fragmentSource))
        {
            m_state = State::OpenFailed;
            return 1;
        }
        const char* fragmentSourceCStr = fragmentSource.c_str();
        glShaderSource(fragmentId, 1, &fragmentSourceCStr, nullptr);

        if (compileShader(fragmentId))
        {
            m_state = State::CompileFailed;
            return 1;
        }
    }

    Logger::verb << "Linking shaders" << Logger::End;

    // Create program and link shaders
    {
        m_shaderProgramId = glCreateProgram();
        glAttachShader(m_shaderProgramId, vertexId);
        glAttachShader(m_shaderProgramId, fragmentId);
        glLinkProgram(m_shaderProgramId);
        int linkStatus;
        glGetProgramiv(m_shaderProgramId, GL_LINK_STATUS, &linkStatus);
        if (!linkStatus)
        {
            char errMsg[1024]{};
            glGetProgramInfoLog(m_shaderProgramId, 1024, nullptr, errMsg);
            Logger::err << "Failed to link shaders:\n" << errMsg << Logger::End;
            m_state = State::LinkFailed;
            return 1;
        }
    }

    m_state = State::Ok;
    return 0;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_shaderProgramId);
    Logger::verb << "Deleted a shader program (" << this << ')' << Logger::End;
}

